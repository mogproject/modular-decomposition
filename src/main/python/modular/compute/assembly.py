from collections import deque
from typing import Any
from modular.tree.RootedForest import RootedForest, Node
from modular.compute.MDComputeNode import MDComputeNode, OperationType

VertexId = Any


def assemble(
    tree: RootedForest[MDComputeNode],
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    alpha_list: dict[VertexId, set[VertexId]],
    prob: Node[MDComputeNode]
) -> None:
    assert not prob.is_leaf()

    # build permutation
    ps: list[Node[MDComputeNode]] = []  # target problems
    current_pivot = prob.data.vertex
    pivot_index = -1

    for p in prob.get_children():
        if p.data.vertex == current_pivot:
            pivot_index = len(ps)
        ps += [p]

    assert pivot_index >= 0, 'roots must include a pivot'

    # main logic
    lcocomp = determine_left_cocomp_fragments(ps, pivot_index)
    rcomp = determine_right_comp_fragments(ps, pivot_index)
    rlayer = determine_right_layer_neighbor(vertex_nodes, alpha_list, ps, pivot_index)
    neighbors = compute_fact_perm_edges(vertex_nodes, alpha_list, ps)
    mu = compute_mu(ps, pivot_index, neighbors)
    boundaries = delineate(pivot_index, lcocomp, rcomp, rlayer, mu)
    # print(f'[TRACE] boundaries={boundaries}')

    root = assemble_tree(tree, ps, pivot_index, boundaries)
    remove_degenerate_duplicates(tree, root)

    # replace the problem node with the result
    tree.replace_children(prob, root)

# ===============================================================================
#    Determine flags
# ===============================================================================


def determine_left_cocomp_fragments(ps: list[Node[MDComputeNode]], pivot_index: int) -> list[bool]:
    """Obtains the flags for left cocomponent fragments."""

    ret = [False for _ in ps]
    for i in range(1, pivot_index):
        if ps[i].data.comp_number < 0:
            continue
        ret[i] = ps[i - 1].data.comp_number == ps[i].data.comp_number
    return ret


def determine_right_comp_fragments(ps: list[Node[MDComputeNode]], pivot_index: int) -> list[bool]:
    """Obtains the flags for right component fragments."""

    ret = [False for _ in ps]
    for i in range(pivot_index + 1, len(ps) - 1):
        if ps[i].data.comp_number < 0:
            continue
        ret[i] = ps[i].data.comp_number == ps[i + 1].data.comp_number
    return ret


def determine_right_layer_neighbor(
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    alpha_list: dict[VertexId, set[VertexId]],
    ps: list[Node[MDComputeNode]],
    pivot_index: int
) -> list[bool]:
    """Obtains the flags for the right layer neighbor."""

    ret = [False for _ in ps]

    def f(t: Node[MDComputeNode]) -> bool:
        tn = t.data.tree_number
        for leaf in t.get_leaves():
            for a in alpha_list[leaf.data.vertex]:
                if vertex_nodes[a].data.tree_number > tn:
                    return True
        return False

    for i in range(pivot_index + 1, len(ps)):
        ret[i] = f(ps[i])
    return ret


# ===============================================================================
#    Compute factorized-permutation edges
# ===============================================================================
def compute_fact_perm_edges(
    vertex_nodes: dict[VertexId, Node[MDComputeNode]],
    alpha_list: dict[VertexId, set[VertexId]],
    ps: list[Node[MDComputeNode]]
) -> list[list[int]]:
    k = len(ps)
    neighbors: list[list[int]] = [[] for _ in range(k)]

    # initialize
    elem_size = [0] * k
    for i, p in enumerate(ps):
        for leaf in p.get_leaves():
            leaf.data.comp_number = i  # reset the comp number to index
            elem_size[i] += 1  # increment the element size

    # find joins
    for i, p in enumerate(ps):
        candidates = []
        marks = [0] * k

        for leaf in p.get_leaves():
            for a in alpha_list[leaf.data.vertex]:
                j = vertex_nodes[a].data.comp_number
                candidates += [j]
                marks[j] += 1

        for j in candidates:
            if elem_size[i] * elem_size[j] == marks[j]:
                # found a join between i and j
                neighbors[i] += [j]
                marks[j] = 0  # reset marks so that there will be no duplicates

    return neighbors


# ===============================================================================
#    Compute mu-values
# ===============================================================================
def compute_mu(
    ps: list[Node[MDComputeNode]],
    pivot_index: int,
    neighbors: list[list[int]]
) -> list[int]:
    """Computes the mu-value for each factorizing permutation element."""

    # initialize mu-values
    mu = [pivot_index if i < pivot_index else 0 for i in range(len(ps))]

    # determine mu-values by looking at elements to the left of pivot
    for i in range(pivot_index):
        for j in neighbors[i]:
            # Neighbor to left of pivot is universal to all up to current
            # and also adjacent to current, so mu gets updated to next.
            if mu[j] == i:
                mu[j] = i + 1

            # Current has an edge past previous farthest edge, so we must update mu.
            if j > mu[i]:
                mu[i] = j
    return mu


# ===============================================================================
#    Delineate
# ===============================================================================
class DelineateState:
    def __init__(self, lb: int, rb: int, left_last_in: int, right_last_in: int) -> None:
        self.lb = lb
        self.rb = rb
        self.left_last_in = left_last_in
        self.right_last_in = right_last_in


def delineate(
    pivot_index: int,
    lcocomp: list[bool],
    rcomp: list[bool],
    rlayer: list[bool],
    mu: list[int]
) -> list[tuple[int, int]]:
    """Finds boundaries for each module."""

    st = DelineateState(pivot_index - 1, pivot_index + 1, pivot_index, pivot_index)
    ret = []
    k = len(lcocomp)

    def compose_series(st: DelineateState) -> bool:
        ret = False
        while 0 <= st.lb and mu[st.lb] <= st.right_last_in and not lcocomp[st.lb]:
            ret = True
            st.left_last_in = st.lb
            st.lb -= 1
        return ret

    def compose_parallel(st: DelineateState) -> bool:
        ret = False
        while st.rb < k and st.left_last_in <= mu[st.rb] and not rcomp[st.rb] and not rlayer[st.rb]:
            ret = True
            st.right_last_in = st.rb
            st.rb += 1
        return ret

    def compose_prime(st: DelineateState) -> bool:
        left_q: deque[int] = deque()
        right_q: deque[int] = deque()

        while True:
            left_q.append(st.lb)
            st.left_last_in = st.lb
            st.lb -= 1
            if not lcocomp[st.left_last_in]:
                break

        while left_q or right_q:
            # add elements from the left to the pivot
            while left_q:
                current_left = left_q.popleft()

                # add all elements up to mu
                while st.right_last_in < mu[current_left]:
                    while True:
                        right_q.append(st.rb)
                        st.right_last_in = st.rb
                        st.rb += 1
                        if rlayer[st.right_last_in]:
                            return True
                        if not rcomp[st.right_last_in]:
                            break

            # add elements to the right of the pivot
            while right_q:
                current_right = right_q.popleft()

                while mu[current_right] < st.left_last_in:
                    while True:
                        left_q.append(st.lb)
                        st.left_last_in = st.lb
                        st.lb -= 1
                        if not lcocomp[st.left_last_in]:
                            break
        return False

    def f(st: DelineateState):
        # (1) if a series module is possible, greedily add the elements composing it
        if compose_series(st):
            return

        # (2) if a parallel module is possible, greedily add the elements composing it
        if compose_parallel(st):
            return

        # (3) must form a prime module
        if compose_prime(st):
            # added to the module an element to the right of x with an edge to a layer to its right,
            # so the module must be the entire graph in this case
            st.left_last_in = 0
            st.right_last_in = k - 1
            st.lb = st.left_last_in - 1
            st.rb = st.right_last_in + 1

    while 0 <= st.lb and st.rb < k:
        f(st)
        ret += [(st.left_last_in, st.right_last_in)]  # delineate the module just found

    return ret


# ===============================================================================
#    Assemble tree
# ===============================================================================
def assemble_tree(
    tree: RootedForest[MDComputeNode],
    ps: list[Node[MDComputeNode]],
    pivot_index: int,
    boundaries: list[tuple[int, int]]
) -> Node[MDComputeNode]:
    k = len(ps)
    lb = pivot_index - 1
    rb = pivot_index + 1
    last_module = ps[pivot_index]

    sz = len(boundaries)
    i = 0  # boundary index

    while 0 <= lb or rb < k:
        lbound, rbound = boundaries[i] if i < sz else (0, k - 1)
        i += 1

        # create spine
        new_module = tree.create_node(MDComputeNode.new_operation_node(OperationType.PRIME))
        tree.move_to(last_module, new_module)

        added_nbrs = False
        added_nonnbrs = False

        # add the subtrees of the new module from the neighbors of x
        while lb >= lbound:
            added_nbrs = True
            tree.move_to(ps[lb], new_module)
            lb -= 1

        # add the subtrees of the new module from the anti-neighbors of x
        while rb <= rbound:
            added_nonnbrs = True
            tree.move_to(ps[rb], new_module)
            rb += 1

        if added_nbrs and added_nonnbrs:
            new_module.data.op_type = OperationType.PRIME
        elif added_nbrs:
            new_module.data.op_type = OperationType.SERIES
        else:
            new_module.data.op_type = OperationType.PARALLEL
        last_module = new_module

    return last_module


# ===============================================================================
#    Cleaning
# ===============================================================================
def remove_degenerate_duplicates(tree: RootedForest[MDComputeNode], node: Node[MDComputeNode]):
    op_type = node.data.op_type

    for c in node.get_children():
        remove_degenerate_duplicates(tree, c)
        if c.data.op_type == op_type and op_type != OperationType.PRIME:
            tree.replace_by_children(c)
            tree.remove(c)
