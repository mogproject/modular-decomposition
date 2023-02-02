from typing import Optional

from modular.tree.RootedForest import RootedForest, Node
from modular.compute.MDComputeNode import MDComputeNode, SplitDirection


def promote(tree: RootedForest[MDComputeNode], prob: Node[MDComputeNode]):
    promote_one_direction(tree, prob, SplitDirection.LEFT)
    promote_one_direction(tree, prob, SplitDirection.RIGHT)


def promote_one_direction(tree: RootedForest[MDComputeNode], prob: Node[MDComputeNode], split_type: SplitDirection):
    for c in prob.get_children():
        promote_one_node(tree, c, split_type)


def promote_one_node(tree: RootedForest[MDComputeNode], node: Node[MDComputeNode], split_type: SplitDirection):
    # non-recursive implementation

    if node.first_child is None:
        return

    st = [(False, node), (True, node.first_child)]
    while st:
        is_forward, nd = st.pop()
        if is_forward:
            # forward pass
            if nd.right:
                st += [(True, nd.right)]  # point to the next node
            if nd.data.is_split_marked(split_type):
                assert nd.parent is not None
                if split_type == SplitDirection.LEFT:
                    tree.move_to_before(nd, nd.parent)
                else:
                    tree.move_to_after(nd, nd.parent)
                if nd.first_child is not None:
                    st += [(False, nd), (True, nd.first_child)]  # dig into the children
        else:
            # backward pass
            if nd.is_leaf() and nd.data.is_operation_node():
                tree.remove(nd)
            elif nd.has_only_one_child():
                tree.replace_by_children(nd)
                tree.remove(nd)
