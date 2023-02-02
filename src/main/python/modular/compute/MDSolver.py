from typing import Any, Optional
from collections import defaultdict

import networkx as nx

from modular.tree.RootedForest import RootedForest, Node
from modular.MDNode import MDNode, OperationType
from modular.compute.MDComputeNode import MDComputeNode, NodeType
from modular.compute.pivot import process_neighbors, do_pivot
from modular.compute.misc import remove_extra_components, remove_layers, complete_alpha_lists, merge_components
from modular.compute.refinement import refine
from modular.compute.promotion import promote
from modular.compute.assembly import assemble

VertexId = Any


def trace(message: str, *args: object) -> None:
    print(f'[TRACE] {message}', *args)


def compute(
    G: nx.Graph,
    tree: RootedForest[MDComputeNode],
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    main_prob: Node[MDComputeNode]
) -> Node[MDComputeNode]:
    current_prob: Optional[Node[MDComputeNode]] = main_prob

    alpha_list: dict[VertexId, set[VertexId]] = defaultdict(set)
    visited: set[VertexId] = set()
    result = None
    t = 0

    while current_prob:
        t += 1
        # trace(f'solve start: current ({t}): {current_prob}, tree={tree}, visited={visited}, alpha={dict(alpha_list)}')
        fc = current_prob.first_child
        assert fc is not None

        current_prob.data.active = True

        if fc.data.node_type != NodeType.PROBLEM:
            # first, needs to solve subproblems
            visited.add(fc.data.vertex)

            if current_prob.has_only_one_child():
                # base case
                process_neighbors(G, tree, vertex_nodes, alpha_list, visited, fc.data.vertex, current_prob, None)
            else:
                # pivot at the first child
                pivoted = do_pivot(G, tree, vertex_nodes, alpha_list, visited, current_prob, fc.data.vertex)

                # dig into the first subproblem
                assert pivoted.first_child is not None
                current_prob = pivoted.first_child
                continue
        else:
            # print(f'[TRACE] solve middle: current ({t}): {current_prob}, tree={tree}, visited={visited}, alpha={dict(alpha_list)}')
            # now, ready to compute this problem
            extra_components = remove_extra_components(tree, current_prob)

            remove_layers(tree, current_prob)
            complete_alpha_lists(current_prob, alpha_list)
            # trace(f'alpha={dict(alpha_list)}, current={tree}')
            refine(tree, vertex_nodes, alpha_list, current_prob)
            # trace(f'after refine: current={current_prob}')
            promote(tree, current_prob)
            # trace(f'after promote: current={current_prob}')
            assemble(tree, vertex_nodes, alpha_list, current_prob)
            # trace(f'after assemble: current={current_prob}')
            merge_components(tree, current_prob, extra_components)
            # trace(f'after merge_components: current={current_prob}')

            # clear all but visited
            assert current_prob.first_child is not None
            for c in current_prob.first_child.dfs_reverse_preorder_nodes():
                if c.is_leaf() and c.data.vertex in alpha_list:
                    del alpha_list[c.data.vertex]
                c.data.clear()

        # trace(f'solve finish: current ({t}): {current_prob}, tree={tree}, visited={visited}, alpha={dict(alpha_list)}')
        result = current_prob.first_child
        current_prob = current_prob.parent if current_prob.is_last_child() else current_prob.right

        assert len(tree.roots) == 1, f'unclean tree: {tree.roots}'

    # set new root of the tree
    assert result is not None
    result_parent = result.parent
    assert result_parent is not None

    tree.detach(result)
    tree.remove(result_parent)
    return result


class MDSolver:
    @staticmethod
    def compute(G: nx.Graph) -> tuple[RootedForest[MDNode], Node[MDNode], list[VertexId]]:
        n = len(G)
        assert n > 0, 'empty graph'

        # Convert node names to consecutive integers starting from 0.
        # The original labels are preserved as the `label` attribute for each node.
        G = nx.convert_node_labels_to_integers(G, first_label=0, ordering='sorted', label_attribute='label')
        labels = nx.get_node_attributes(G, 'label')

        # build computation tree
        tree: RootedForest[MDComputeNode] = RootedForest()

        # create the main problem
        main_prob = tree.create_node(MDComputeNode.new_problem_node(connected=False))

        # add vertex nodes (cannot be removed)
        vertex_nodes = {}
        for v in reversed(list(G.nodes())):
            node = tree.create_node(MDComputeNode.new_vertex_node(v))
            vertex_nodes[v] = node
            tree.move_to(node, main_prob)

        # main logic
        comp_root = compute(G, tree, vertex_nodes, main_prob)

        # create result tree
        result_tree: RootedForest[MDNode] = RootedForest()
        mapping: dict[Node[MDComputeNode], Node[MDNode]] = {}
        vertices = []

        # create vertex nodes in DFS ordering (right-to-left)
        for i, comp_node in enumerate(comp_root.get_leaves()):
            v = labels[comp_node.data.vertex]
            vertices += [v]
            nd = result_tree.create_node(MDNode(v, OperationType.PRIME, i, i + 1))
            mapping[comp_node] = nd

        # create internal nodes from the bottom
        for comp_node in reversed(list(comp_root.bfs_nodes())):
            if comp_node.data.is_vertex_node():
                continue
            assert comp_node.data.is_operation_node(), 'there should not be a problem node'

            children = comp_node.get_children()
            idx_begin = min(mapping[c].data.vertices_begin for c in children)
            idx_end = max(mapping[c].data.vertices_end for c in children)
            nd = result_tree.create_node(MDNode(op=comp_node.data.op_type, vertices_begin=idx_begin, vertices_end=idx_end))
            for c in reversed(children):
                result_tree.move_to(mapping[c], nd)
            mapping[comp_node] = nd

        return result_tree, mapping[comp_root], vertices
