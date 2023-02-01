from __future__ import annotations
from collections import deque
from typing import Generator, Optional, TypeVar, Generic

T = TypeVar('T')  # declare type variable


class Node(Generic[T]):
    """
    Represents a node in the tree/forest.
    """

    def __init__(
        self,
        data: T,
        parent: Optional[Node[T]] = None,
        left: Optional[Node[T]] = None,
        right: Optional[Node[T]] = None,
        first_child: Optional[Node[T]] = None,
        num_children: int = 0
    ) -> None:
        self.data = data
        self.parent = parent
        self.left = left
        self.right = right
        self.first_child = first_child
        self.num_children = num_children  # number of children

    # I/O
    def __str__(self) -> str:
        """Recursively prints the subtree."""
        return '(' + str(self.data) + ''.join(str(c) for c in self.get_children()) + ')'

    def __repr__(self) -> str:
        """Recursively prints the subtree."""
        return '(' + repr(self.data) + ''.join(repr(c) for c in self.get_children()) + ')'

    # Utilities
    def is_root(self) -> bool: return self.parent is None
    def has_parent(self) -> bool: return self.parent is not None
    def is_first_child(self) -> bool: return self.has_parent() and self.left is None
    def is_last_child(self) -> bool: return self.has_parent() and self.right is None
    def is_leaf(self) -> bool: return self.first_child is None
    def has_child(self) -> bool: return self.first_child is not None
    def has_only_one_child(self) -> bool: return self.num_children == 1
    def number_of_children(self) -> int: return self.num_children

    def get_children(self) -> list[Node[T]]:
        """Returns a list of the (direct) children of this node."""
        x = self.first_child
        ret = []
        while x:
            ret += [x]
            x = x.right
        return ret

    def get_leaves(self) -> list[Node[T]]:
        """Returns a list of the leaves of this subtree."""
        ret: list[Node[T]] = []
        st: list[Node[T]] = [self]

        while st:
            x = st.pop()
            if x.is_leaf():
                ret += [x]
            else:
                st += x.get_children()  # result will be right-to-left
        return ret

# ==============================================================================
#    Node Traversal
# ==============================================================================
    def bfs_nodes(self) -> Generator[Node[T], None, None]:
        """
        Returns a generator of nodes in a breadth-first-search ordering
        starting at this node.
        """
        q: deque[Node[T]] = deque()
        q.append(self)

        while q:
            x = q.popleft()
            yield x
            q.extend(x.get_children())

    def dfs_preorder_nodes(self) -> Generator[Node[T], None, None]:
        """
        Returns a generator of nodes in a depth-first-search (left to right)
        pre-ordering starting at this node.
        """
        st: list[Node[T]] = [self]

        while st:
            x = st.pop()
            yield x
            st += reversed(x.get_children())

    def dfs_reverse_preorder_nodes(self) -> Generator[Node[T], None, None]:
        """
        Returns a generator of nodes in a depth-first-search reverse (right to left)
        pre-ordering starting at this node.
        """
        st: list[Node[T]] = [self]

        while st:
            x = st.pop()
            yield x
            st += x.get_children()

    def get_ancestors(self) -> Generator[Node[T], None, None]:
        """Returns a generator of the ancestors (bottom to top) of this node."""
        p = self.parent
        while p:
            yield p
            p = p.parent

    def get_root(self) -> Node[T]:
        """Returns the root of this node."""
        ret = None
        p: Optional[Node[T]] = self
        while p:
            ret = p
            p = p.parent
        assert ret is not None
        return ret


class RootedForest(Generic[T]):
    """
    Generic rooted forest (disjoint set of rooted trees).
    """

    def __init__(self) -> None:
        self.roots: set[Node[T]] = set()
        self.size = 0

    def __len__(self) -> int: return self.size
    def __bool__(self) -> bool: return bool(self.roots)
    def __repr__(self) -> str: return repr(self.roots)

# ==============================================================================
#    Node Addition and Removal
# ==============================================================================
    def create_node(self, data: T) -> Node[T]:
        node = Node(data)
        self.roots.add(node)
        self.size += 1
        return node

    def remove(self, node: Node[T]) -> None:
        """Removes the leaf node from the tree."""
        assert node.is_leaf(), 'remove: must be a leaf'

        self.size -= 1
        self.detach(node)
        self.roots.remove(node)


# ==============================================================================
#    Modification
# ==============================================================================


    def detach(self, node: Node[T]) -> None:
        """Detaches the node from its parent and adds it to the root set."""
        if node.is_root():
            return

        assert node.parent is not None
        node.parent.num_children -= 1
        if node.is_first_child():
            node.parent.first_child = node.right
        if node.left:
            node.left.right = node.right
        if node.right:
            node.right.left = node.left

        node.parent = None
        node.left = None
        node.right = None
        self.roots.add(node)

    def move_to(self, node: Node[T], new_parent: Node[T]) -> None:
        """Detaches the node and moves it to the new parent as the first child."""
        assert node != new_parent, 'move_to: node and new_parent cannot be the same'

        self.detach(node)
        self._add_child(new_parent, node)
        self.roots.remove(node)

    def swap(self, a: Node[T], b: Node[T]) -> None:
        assert a.get_root() != b.get_root(), 'swap: a and b must belong to different trees'

        for x, y in [(a, b), (b, a)]:
            if x.is_first_child():
                assert x.parent is not None
                x.parent.first_child = y
            if x.left:
                x.left.right = y
            if x.right:
                x.right.left = y
            if x.is_root() and not y.is_root():
                self.roots.remove(x)
                self.roots.add(y)

        a.parent, b.parent = b.parent, a.parent
        a.left, b.left = b.left, a.left
        a.right, b.right = b.right, a.right

    def replace(self, node: Node[T], replace_by: Node[T]) -> None:
        """Replaces the node and its subtree with the given node."""
        assert node != replace_by, 'replace: replace_by must differ from node'
        assert replace_by not in node.get_ancestors(), 'replace: replace_by cannot be an ancestor of node'

        self.detach(replace_by)
        self.swap(node, replace_by)

    def move_to_before(self, node: Node[T], target: Node[T]) -> None:
        """Moves the node to the left sibling of the given target."""
        assert target.parent is not None, 'move_to_before: target must not be a root'
        assert node != target, 'move_to_before: node and target must differ'
        assert node not in target.get_ancestors(), 'move_to_before: target cannot be an ancestor of node'

        self.detach(node)
        self.roots.remove(node)
        node.parent = target.parent
        node.left = target.left
        node.right = target

        target.parent.num_children += 1
        if target.is_first_child():
            target.parent.first_child = node
        else:
            assert target.left is not None
            target.left.right = node
        target.left = node

    def move_to_after(self, node: Node[T], target: Node[T]) -> None:
        """Moves the node to the right sibling of the given target."""
        assert target.parent is not None, 'move_to_after: target must not be a root'
        assert node != target, 'move_to_after: node and target must differ'
        assert node not in target.get_ancestors(), 'move_to_after: target cannot be an ancestor of node'

        self.detach(node)
        self.roots.remove(node)
        node.parent = target.parent
        node.left = target
        node.right = target.right

        target.parent.num_children += 1
        if target.right:
            target.right.left = node
        target.right = node

    def make_first_child(self, node: Node[T]) -> None:
        """Moves the node to the first among all its siblings."""
        if not node.is_root() and not node.is_first_child():
            assert node.parent is not None
            assert node.parent.first_child is not None
            self.move_to_before(node, node.parent.first_child)

    def add_children_from(self, node: Node[T], target: Node[T]) -> None:
        """Moves all the children of the target to node."""
        assert target not in node.get_ancestors(), 'add_children_from: target cannot be an ancestor of node'

        if node == target:
            return  # do nothing

        for c in target.get_children():
            c.parent = node
            if c.is_last_child():
                c.right = node.first_child
                if node.has_child():
                    assert node.first_child is not None
                    node.first_child.left = c
                break

        if target.has_child():
            node.first_child = target.first_child
        node.num_children += target.num_children
        target.first_child = None
        target.num_children = 0

    def replace_by_children(self, node: Node[T]) -> None:
        """Replaces the node by its children."""
        assert not node.is_root(), 'replace_by_children: node must not be a root'

        for c in node.get_children():
            self.move_to_before(c, node)
        self.detach(node)

    def replace_children(self, node: Node[T], target: Node[T]) -> None:
        """Replaces the children of the node with target. The children are detached (but not removed)."""
        for c in node.get_children():
            self.detach(c)
        self.move_to(target, node)

    def _add_child(self, parent: Node[T], child: Node[T]) -> None:
        assert child.is_root(), '_add_child: child must be a root'

        if parent.has_child():
            assert parent.first_child is not None
            parent.first_child.left = child
            child.right = parent.first_child

        parent.first_child = child
        child.parent = parent
        parent.num_children += 1
