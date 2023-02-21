from collections import defaultdict, deque
from typing import Any
from modular.tree.RootedForest import RootedForest, Node
from modular.compute.MDComputeNode import MDComputeNode, OperationType, SplitDirection

__all__ = ['refine']

VertexId = Any


def trace(message: str, *args: object) -> None:
    print(f'[TRACE] {message}', *args)


def refine(
    tree: RootedForest[MDComputeNode],
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    alpha_list: dict[VertexId, set[VertexId]],
    prob: Node[MDComputeNode]
) -> None:
    number_by_comp(prob)
    number_by_tree(prob)

    # trace('comp:', {v: v.data.comp_number for v in prob.bfs_nodes()})
    # trace('tree:', {v: v.data.tree_number for v in prob.bfs_nodes()})

    for v in prob.get_leaves():
        refine_with(tree, vertex_nodes, alpha_list, v.data.vertex, prob.data.vertex)
        # trace(f'refined at {v}: tree={repr(tree)}')

# ===============================================================================
#    Set up
# ===============================================================================


def number_by_comp(prob: Node[MDComputeNode]) -> None:
    """Updtaes the component number for each node in the given problem subtree."""
    comp_number = 0
    pivot = prob.data.vertex
    op_type: OperationType = OperationType.SERIES

    for c in prob.get_children():
        if c.data.vertex == pivot:
            op_type = OperationType.PARALLEL

        if c.data.op_type == op_type:
            for x in c.get_children():
                for y in x.dfs_reverse_preorder_nodes():
                    y.data.comp_number = comp_number
                comp_number += 1
        else:
            for y in c.dfs_reverse_preorder_nodes():
                y.data.comp_number = comp_number
            comp_number += 1


def number_by_tree(prob: Node[MDComputeNode]) -> None:
    """Updtaes the tree number for each node in the given problem subtree."""
    tree_number = 0
    for c in prob.get_children():
        for y in c.dfs_reverse_preorder_nodes():
            y.data.tree_number = tree_number
        tree_number += 1


# ===============================================================================
#    Utilities
# ===============================================================================
def is_root_operator(node: Node[MDComputeNode]) -> bool:
    return node.parent is None or not node.parent.data.is_operation_node()


# ===============================================================================
#    Marking split types
# ===============================================================================
def mark_ancestors_by_split(node: Node[MDComputeNode], split_type: SplitDirection) -> None:
    """Adds the given mark to all of the node's ancestors."""
    p = node.parent
    while p is not None:
        if p.data.is_problem_node():
            break  # passed the operation root
        if p.data.is_split_marked(split_type):
            add_split_mark(p, split_type, should_recurse=True)
            break
        add_split_mark(p, split_type, should_recurse=True)
        p = p.parent


def add_split_mark(node: Node[MDComputeNode], split_type: SplitDirection, should_recurse: bool) -> None:
    """Add the given mark to the node and possibly its children."""
    if not node.data.is_split_marked(split_type):
        if node.parent is not None and node.parent.data.is_operation_node():
            node.parent.data.increment_num_split_children(split_type)
        node.data.set_split_mark(split_type)

    if not should_recurse or node.data.op_type != OperationType.PRIME:
        return

    if node.number_of_children() == node.data.get_num_split_children(split_type):
        return  # split type is already set to all children

    for c in node.get_children():
        if not c.data.is_split_marked(split_type):
            node.data.increment_num_split_children(split_type)
            c.data.set_split_mark(split_type)


# ===============================================================================
#    Get max subtrees
# ===============================================================================
def is_parent_fully_charged(x: Node[MDComputeNode]) -> bool:
    if is_root_operator(x):
        return False
    p = x.parent
    assert p is not None
    return p.number_of_children() == p.data.num_marks


def get_max_subtrees(leaves: list[Node[MDComputeNode]]) -> list[Node[MDComputeNode]]:
    """
    Finds the set of maximal subtrees such that the leaves of each subtree are
    subsets of the given leaf set.
    """
    full_charged = list(leaves)
    charged: list[Node[MDComputeNode]] = []

    # charging
    idx = 0
    while idx < len(full_charged):
        x = full_charged[idx]
        if not is_root_operator(x):
            p = x.get_parent()
            if p.data.num_marks == 0:
                charged += [p]
            p.data.add_mark()

            if p.number_of_children() == p.data.num_marks:
                # fully charged
                full_charged += [p]
        idx += 1

    # discharging
    ret: list[Node[MDComputeNode]] = [x for x in full_charged if not is_parent_fully_charged(x)]

    # cleaning
    for x in charged:
        x.data.clear_marks()
    return ret


# ===============================================================================
#    Group sibling nodes
# ===============================================================================
def group_sibling_nodes(
    tree: RootedForest[MDComputeNode],
    nodes: list[Node[MDComputeNode]]
) -> list[tuple[Node[MDComputeNode], bool]]:
    """
    Precondition: `nodes` must be maximal subtrees (no node is an ancestor of another).
    """
    parents: list[Node[MDComputeNode]] = []
    sibling_groups: list[tuple[Node[MDComputeNode], bool]] = []

    for node in nodes:
        if is_root_operator(node):
            # (1) roots of trees
            sibling_groups += [(node, False)]
        else:
            tree.make_first_child(node)
            p = node.parent
            assert p is not None

            if p.data.num_marks == 0:
                parents += [p]
            p.data.add_mark()

    for p in parents:
        # there must be at least one mark
        c = p.first_child
        assert c is not None

        if p.data.num_marks == 1:
            # (2) non-root nodes without siblings
            sibling_groups += [(c, False)]
        else:
            # (3) group sibling nodes as the children of a newly inserted node
            grouped_children = tree.create_node(p.data.copy())

            for split_type in [SplitDirection.LEFT, SplitDirection.RIGHT]:
                if grouped_children.data.is_split_marked(split_type):
                    p.data.increment_num_split_children(split_type)

            # FIX FROM ORIGINAL CODE (RecSubProblem.java line 971); bug when a child of `p` is also in `parents`
            for c in p.get_children()[:p.data.num_marks]:
                for split_type in [SplitDirection.LEFT, SplitDirection.RIGHT]:
                    if c.data.is_split_marked(split_type):
                        p.data.decrement_num_split_children(split_type)
                        grouped_children.data.increment_num_split_children(split_type)

                tree.move_to(c, grouped_children)
            tree.move_to(grouped_children, p)

            sibling_groups += [(grouped_children, grouped_children.data.op_type == OperationType.PRIME)]

        # clean marks
        p.data.clear_marks()

    return sibling_groups


# ===============================================================================
#    Subroutine
# ===============================================================================
def get_split_type(
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    refiner: VertexId,
    pivot: VertexId,
    node: Node[MDComputeNode]
) -> SplitDirection:
    pivot_tn = vertex_nodes[pivot].data.tree_number
    refiner_tn = vertex_nodes[refiner].data.tree_number
    current_tn = node.data.tree_number
    return SplitDirection.LEFT if current_tn < pivot_tn or refiner_tn < current_tn else SplitDirection.RIGHT


def refine_one_node(
    tree: RootedForest[MDComputeNode],
    node: Node[MDComputeNode],
    split_type: SplitDirection,
    new_prime: bool,
) -> None:
    if is_root_operator(node):
        return

    p = node.parent
    new_sibling = None

    assert p is not None

    if is_root_operator(p):
        # parent is a root; must split the tree
        if split_type == SplitDirection.LEFT:
            tree.move_to_before(node, p)
        else:
            tree.move_to_after(node, p)
        for st in [SplitDirection.LEFT, SplitDirection.RIGHT]:
            if node.data.is_split_marked(st):
                p.data.decrement_num_split_children(st)

        new_sibling = p

        if p.has_only_one_child():
            tree.replace_by_children(p)
            tree.remove(p)
            new_sibling = None
        else:
            assert not p.is_leaf()

    elif p.data.op_type != OperationType.PRIME:
        # parent is not a root or PRIME
        assert p.parent is not None
        assert p.number_of_children() > 1, 'p should have multiple children at this point'

        replacement = tree.create_node(p.data.copy())
        tree.replace(p, replacement)
        tree.move_to(node, replacement)
        tree.move_to(p, replacement)
        new_sibling = p

        for st in [SplitDirection.LEFT, SplitDirection.RIGHT]:
            if node.data.is_split_marked(st):
                p.data.decrement_num_split_children(st)
                replacement.data.increment_num_split_children(st)
            if p.data.is_split_marked(st):
                replacement.data.increment_num_split_children(st)

    add_split_mark(node, split_type, should_recurse=new_prime)
    mark_ancestors_by_split(node, split_type)

    if new_sibling:
        # non-prime or a new root; safe to set should_recurse=True
        add_split_mark(new_sibling, split_type, should_recurse=True)


def refine_with(
    tree: RootedForest[MDComputeNode],
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    alpha_list: dict[VertexId, set[VertexId]],
    refiner: VertexId,
    pivot: VertexId
) -> None:
    subtree_roots = get_max_subtrees([vertex_nodes[x] for x in alpha_list[refiner]])
    sibling_groups = group_sibling_nodes(tree, subtree_roots)
    # trace(f'refiner={refiner}, subtree_roots={subtree_roots}, sibling_groups={sibling_groups}, tree={tree}')

    for current, new_prime in sibling_groups:
        # determine the split type
        split_type = get_split_type(vertex_nodes, refiner, pivot, current)
        # trace(f'before: refiner={refiner}, node={current}, tree={tree}')
        refine_one_node(tree, current, split_type, new_prime)
        # trace(f'after : refiner={refiner}, node={current}, tree={tree}')

        # sanity check
        # for r in tree.roots:
        #     for node in r.dfs_reverse_preorder_nodes():
        #         if not node.data.is_operation_node():
        #             assert node.data.num_left_split_children == 0
        #             assert node.data.num_right_split_children == 0
        #         else:
        #             lact = node.data.num_left_split_children
        #             ract = node.data.num_right_split_children
        #             lexp = sum(1 if c.data.is_split_marked(SplitDirection.LEFT) else 0 for c in node.get_children())
        #             rexp = sum(1 if c.data.is_split_marked(SplitDirection.RIGHT) else 0 for c in node.get_children())
        #             assert lact == lexp, f'node={node}, expect={lexp}, actual={lact}'
        #             assert ract == rexp, f'node={node}, expect={rexp}, actual={lact}'
