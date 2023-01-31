from collections import deque, defaultdict

import networkx as nx
import numpy as np

from modular.tree.RootedTree import RootedTree, Node
from modular.util.matrix_util import equivalent_classes
from modular.MDNode import MDNode, VertexId
from modular.OperationType import OperationType


class MDNaiveSolver:
    @staticmethod
    def compute_implication_matrix(G: nx.Graph) -> np.ndarray:
        """
        Creates the modified adjacency matrix that encodes implication classes
        of the given graph.
        """
        n = len(G)
        assert set(G.nodes()) == set(range(n))

        H = G.copy()
        M = np.zeros((n, n), dtype=int)
        label = 0

        for u, v in G.edges():
            uu, vv = min(u, v), max(u, v)
            if M[uu, vv] > 0:
                continue  # already labeled

            label += 1

            q: deque[tuple[int, int]] = deque()
            q.append((uu, vv))
            target = []

            while q:
                a, b = q.popleft()
                assert a < b
                if M[a, b] > 0:
                    continue

                M[a, b] = label
                target += [(a, b)]
                na = set(H.neighbors(a))
                nb = set(H.neighbors(b))

                for c in na - nb:
                    aa, cc = min(a, c), max(a, c)
                    if M[aa, cc] == 0:
                        q.append((aa, cc))
                for d in nb - na:
                    bb, dd = min(b, d), max(b, d)
                    if M[bb, dd] == 0:
                        q.append((bb, dd))

            H.remove_edges_from(target)

        return M + M.T

    @staticmethod
    def compute(G: nx.Graph) -> tuple[RootedTree[MDNode], Node[MDNode], list[VertexId]]:
        # Implements the algorithm described in
        # "A Fast Algorithm for the Decomposition of Graphs and Posets"
        # Hermann Buer and Rolf H. Möhring (1983)

        # Convert node names to consecutive integers starting from 0.
        # The original labels are preserved as the `label` attribute for each node.
        G = nx.convert_node_labels_to_integers(G, first_label=0, ordering='sorted', label_attribute='label')
        labels = nx.get_node_attributes(G, 'label')
        A = MDNaiveSolver.compute_implication_matrix(G)

        n = len(G)
        vertices = list(G.nodes())

        tree: RootedTree[MDNode] = RootedTree()
        root = tree.create_node(MDNode(None, None, 0, n))

        q: deque[Node[MDNode]] = deque()
        q.append(root)

        while q:
            node = q.popleft()

            # Base: vertex node
            if node.data.size() == 1:
                node.data.vertex = labels[vertices[node.data.vertices_begin]]
                continue

            xs = vertices[node.data.vertices_begin:node.data.vertices_end]
            H = nx.induced_subgraph(G, xs)

            if not nx.is_connected(H):
                # Case (1) G[X] is disconnected
                node.data.op = OperationType.PARALLEL
                parts = nx.connected_components(H)
            elif not nx.is_connected(nx.complement(H)):
                # Case (2) complement(G[X]) is disconnected
                node.data.op = OperationType.SERIES
                parts = nx.connected_components(nx.complement(H))
            else:
                # Case (3) prime graph
                node.data.op = OperationType.PRIME

                # find the implication class F such that support(A[X]_F) = |X|
                target_class = None
                cnt: dict[int, int] = defaultdict(int)
                for u in xs:
                    for k in set(A[u, xs]):  # check if every row contains at least one k
                        if k == 0:
                            continue
                        cnt[k] += 1
                        if cnt[k] == len(xs):
                            target_class = k
                            break

                assert target_class is not None, 'there must be exactly one such class'

                # compute the adjacency classes of F
                adj_classes = equivalent_classes(A[xs, :][:, xs] == target_class)
                parts = ([xs[i] for i in adj] for adj in adj_classes)

            index = node.data.vertices_begin

            for vs in parts:
                vertices[index:index + len(vs)] = list(vs)  # reorder vertices
                child = tree.create_node(MDNode(None, None, index, index + len(vs)))
                tree.move_to(child, node)
                q.append(child)
                index += len(vs)

        return tree, root, [labels[i] for i in vertices]
