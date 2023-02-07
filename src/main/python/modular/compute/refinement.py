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

    # trace('comp:', {v: v.data.comp_number for v in prob.get_subnodes()})
    # trace('tree:', {v: v.data.tree_number for v in prob.get_subnodes()})

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
    for p in node.get_ancestors():
        if p.data.is_problem_node():
            break  # passed the operation root
        add_split_mark(p, split_type, should_recurse=True)


def add_split_mark(node: Node[MDComputeNode], split_type: SplitDirection, should_recurse: bool) -> None:
    """Add the given mark to the node and possibly its children."""
    node.data.set_split_mark(split_type)

    if should_recurse and node.data.op_type == OperationType.PRIME:
        for c in node.get_children():
            if not c.data.is_split_marked(split_type):
                c.data.set_split_mark(split_type)


# ===============================================================================
#    Get max subtrees
# ===============================================================================
def get_max_subtrees(leaves: list[Node[MDComputeNode]]) -> list[Node[MDComputeNode]]:
    """
    Finds the set of maximal subtrees such that the leaves of each subtree are
    subsets of the given leaf set.
    """

    num_charges: dict[Node[MDComputeNode], int] = defaultdict(int)

    # charging
    fully_charged: set[Node[MDComputeNode]] = set(leaves)
    st = [x for x in leaves]

    while st:
        x = st.pop()
        if not is_root_operator(x):
            p = x.parent
            assert p is not None
            num_charges[p] += 1
            if p.number_of_children() == num_charges[p]:
                # fully charged
                fully_charged.add(p)

                # discharge children
                for c in p.get_children():
                    fully_charged.remove(c)

                st += [p]

    return list(fully_charged)


# ===============================================================================
#    Group sibling nodes
# ===============================================================================
def group_sibling_nodes(
    tree: RootedForest[MDComputeNode],
    nodes: list[Node[MDComputeNode]]
) -> list[tuple[Node[MDComputeNode], bool]]:
    num_marks: dict[Node[MDComputeNode], int] = defaultdict(int)
    parents: list[Node[MDComputeNode]] = []

    for node in nodes:
        num_marks[node] += 1

        if not is_root_operator(node):
            tree.make_first_child(node)
            p = node.parent
            assert p is not None

            if p not in num_marks:
                parents += [p]
            num_marks[p] += 1

    sibling_groups: list[tuple[Node[MDComputeNode], bool]] = []

    # (1) roots of trees
    for node in nodes:
        if is_root_operator(node):
            del num_marks[node]
            sibling_groups += [(node, False)]

    for p in parents:
        # there must be at least one mark
        c = p.first_child
        assert c is not None

        if num_marks[p] == 1:
            # (2) non-root nodes without siblings
            sibling_groups += [(c, False)]
        else:
            # (3) group sibling nodes as the children of a newly inserted node
            grouped_children = tree.create_node(p.data.copy())

            # FIX FROM ORIGINAL CODE (RecSubProblem.java line 971); bug when a child of `p` is also in `parents`
            for c in p.get_children()[:num_marks[p]]:
                tree.move_to(c, grouped_children)
            tree.move_to(grouped_children, p)

            sibling_groups += [(grouped_children, grouped_children.data.op_type == OperationType.PRIME)]

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
