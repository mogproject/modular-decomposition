import networkx as nx
from collections import deque
from typing import Optional

from modular.MDNode import MDNode, VertexId
from modular.tree.RootedForest import Node
from modular.compute.MDSolver import MDSolver
from modular.compute_naive.MDNaiveSolver import MDNaiveSolver


__all__ = ['modular_decomposition']


class MDTree:
    """
    Modular decomposition tree for undirected graphs.
    """

    def __init__(self, G: nx.Graph, solver: str = 'naive') -> None:
        assert len(G) > 0, 'graph cannot be empty'

        if solver == 'linear':
            tree, root, vertices = MDSolver.compute(G)
        elif solver == 'naive':
            tree, root, vertices = MDNaiveSolver.compute(G)
        else:
            raise ValueError(f'Unknown solver: {solver}')

        self.tree = tree
        self.root = root
        self.vertices = vertices

    def sort(self) -> None:
        """Sorts all nodes in lexicographical order."""

        level_order = list(self.root.bfs_nodes())

        # first pass (bottom-up): find the (lexicographically) smallest vertex for each module
        min_label: dict[Node[MDNode], VertexId] = {}
        for node in reversed(level_order):
            if node.is_leaf():
                min_label[node] = node.data.vertex
            
            if node.parent is not None:
                min_label[node.parent] = min(min_label[node.parent], min_label[node]) if node.parent in min_label else min_label[node]

        # second pass (top-down): reorder nodes and vertices
        new_begin: dict[Node[MDNode], int] = {self.root: 0}

        for node in level_order:
            if node.is_leaf():
                self.vertices[new_begin[node]] = node.data.vertex
                continue

            idx = new_begin[node] + node.data.size()
            for _, c in sorted(((min_label[c], c) for c in node.get_children()), reverse=True):
                idx -= c.data.size()
                new_begin[c] = idx
                self.tree.make_first_child(c)

    def modular_width(self) -> int:
        """Returns the modular-width."""

        ret = 0
        for c in self.root.dfs_reverse_preorder_nodes():
            if c.data.is_prime_node():
                ret = max(ret, c.number_of_children())
        return ret

    def __repr__(self) -> str:
        return repr(self.root)


def modular_decomposition(G: nx.Graph, sorted: bool = False, solver: str = 'naive') -> Optional[MDTree]:
    """Alias to the constructor."""

    if len(G) == 0:
        return None
    ret = MDTree(G, solver=solver)
    if sorted:
        ret.sort()
    return ret
