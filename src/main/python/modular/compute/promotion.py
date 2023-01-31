from modular.tree.RootedTree import RootedTree, Node
from modular.compute.MDComputeNode import MDComputeNode, SplitDirection

def promote(tree: RootedTree[MDComputeNode], prob: Node[MDComputeNode]):
    promote_one_direction(tree, prob, SplitDirection.LEFT)
    promote_one_direction(tree, prob, SplitDirection.RIGHT)


def promote_one_direction(tree: RootedTree[MDComputeNode], prob: Node[MDComputeNode], split_type: SplitDirection):
    for c in prob.get_children():
        promote_one_node(tree, c, split_type)


def promote_one_node(tree: RootedTree[MDComputeNode], node: Node[MDComputeNode], split_type: SplitDirection):
    to_promote = node.first_child
    while to_promote:
        # print(f'[TRACE] promote node={node}, split={split_type}, node split={node.data.split_type}')
        nxt = to_promote.right
        if to_promote.data.is_split_marked(split_type):
            if split_type == SplitDirection.LEFT:
                tree.move_to_before(to_promote, node)
            else:
                tree.move_to_after(to_promote, node)
            promote_one_node(tree, to_promote, split_type)
        to_promote = nxt

    if node.is_leaf() and node.data.is_operation_node():
        tree.remove(node)
    elif node.has_only_one_child():
        tree.replace_by_children(node)
        tree.remove(node)
