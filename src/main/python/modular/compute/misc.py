from typing import Any, Optional
import networkx as nx
from modular.tree.RootedForest import RootedForest, Node
from modular.compute.MDComputeNode import MDComputeNode, OperationType

VertexId = Any


def remove_extra_components(tree: RootedForest[MDComputeNode], prob: Node[MDComputeNode]) -> Optional[Node[MDComputeNode]]:
    subprob = prob.first_child
    while subprob and subprob.data.connected:
        subprob = subprob.right

    ret = None
    if subprob:
        ret = subprob.first_child
        assert ret is not None
        tree.detach(ret)
        assert subprob.is_leaf()
        tree.remove(subprob)

    return ret


def remove_layers(tree: RootedForest[MDComputeNode], prob: Node[MDComputeNode]):
    for c in prob.get_children():
        tree.replace_by_children(c)
        tree.remove(c)


def complete_alpha_lists(prob: Node[MDComputeNode], alpha_list: dict[VertexId, set[VertexId]]):
    """Makes the alpha lists in this subproblem symmetric (and irredundant)."""

    for v in prob.get_leaves():
        for a in alpha_list[v.data.vertex]:
            alpha_list[a].add(v.data.vertex)


def merge_components(tree: RootedForest[MDComputeNode], prob: Node[MDComputeNode], new_components: Optional[Node[MDComputeNode]]):
    if new_components is None:
        return

    fc = prob.first_child
    assert fc is not None

    if new_components.data.op_type == OperationType.PARALLEL:
        if fc.data.op_type == OperationType.PARALLEL:
            tree.add_children_from(new_components, fc)
        else:
            tree.move_to(fc, new_components)
        tree.move_to(new_components, prob)
    else:
        new_root = tree.create_node(MDComputeNode.new_operation_node(OperationType.PARALLEL))

        tree.move_to(new_root, prob)
        tree.move_to(new_components, new_root)
        tree.move_to(fc, new_root)
