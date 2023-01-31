from typing import Any, Optional
import networkx as nx
from modular.tree.RootedTree import RootedTree, Node
from modular.compute.MDComputeNode import MDComputeNode

__all__ = ['do_pivot', 'process_neighbors']

VertexId = Any


def process_neighbors(
    G: Any,  # nx.Graph
    tree: RootedTree[MDComputeNode],
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    alpha_list: dict[VertexId, set[VertexId]],
    visited: set[VertexId],
    pivot: VertexId,
    current_prob: Node[MDComputeNode],
    nbr_prob: Optional[Node[MDComputeNode]]
) -> None:
    for nbr in G[pivot]:
        if nbr in visited:
            alpha_list[nbr].add(pivot)  # add pivot to nbr's alpha list
        elif vertex_nodes[nbr].parent == current_prob:
            assert nbr_prob is not None
            tree.move_to(vertex_nodes[nbr], nbr_prob)
        else:
            pull_forward(tree, vertex_nodes, nbr)


def is_pivot_layer(node: Node[MDComputeNode]) -> bool:
    if node.parent is None or node.first_child is None:
        return False
    return node.parent.data.is_problem_node() and node.parent.data.vertex == node.first_child.data.vertex


def pull_forward(
    tree: RootedTree[MDComputeNode],
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    v: VertexId
):
    current_layer = vertex_nodes[v].parent
    assert current_layer

    if current_layer.data.connected:
        return
    assert current_layer.data.is_problem_node(), 'pull_forward: not a problem node'

    prev_layer = current_layer.left
    assert prev_layer

    # form a new layer
    if prev_layer.data.active or is_pivot_layer(prev_layer):
        new_layer = tree.create_node(MDComputeNode.new_problem_node(connected=True))
        tree.move_to_before(new_layer, current_layer)
        prev_layer = new_layer

    if prev_layer.data.connected:
        tree.move_to(vertex_nodes[v], prev_layer)

    if current_layer.is_leaf():
        # all leaves in this layer have been removed
        tree.remove(current_layer)


def do_pivot(
    G: Any,  # nx.Graph
    tree: RootedTree[MDComputeNode],
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    alpha_list: dict[VertexId, set[MDComputeNode]],
    visited: set[VertexId],
    prob: Node[MDComputeNode],
    pivot: VertexId
) -> Node[MDComputeNode]:
    # duplicate problem node
    replacement = tree.create_node(prob.data.copy())
    tree.swap(prob, replacement)
    tree.move_to(prob, replacement)
    replacement.data.vertex = pivot  # set pivot

    # clear attributes
    prob.data.active = False
    prob.data.connected = False
    prob.data.vertex = -1

    # create subproblem for the pivot
    pivot_prob = tree.create_node(MDComputeNode.new_problem_node(connected=True))
    tree.move_to(pivot_prob, replacement)
    tree.move_to(vertex_nodes[pivot], pivot_prob)

    # create subproblem for the neighbors of pivot
    nbr_prob = tree.create_node(MDComputeNode.new_problem_node(connected=True))
    tree.move_to(nbr_prob, replacement)
    process_neighbors(G, tree, vertex_nodes, alpha_list, visited, pivot, prob, nbr_prob)

    # clean up
    if prob.is_leaf():
        tree.remove(prob)
    if nbr_prob.is_leaf():
        tree.remove(nbr_prob)

    return replacement
