from __future__ import annotations
from enum import Enum
from modular.OperationType import OperationType


class NodeType(Enum):
    VERTEX = 1
    OPERATION = 2
    PROBLEM = 3


class SplitDirection(Enum):
    NONE = 0
    LEFT = 1
    RIGHT = 2
    MIXED = 3


class MDComputeNode:
    def __init__(
        self,
        node_type: NodeType,
        op_type: OperationType = OperationType.PRIME,
        split_type: SplitDirection = SplitDirection.NONE,
        vertex: int = -1,
        comp_number: int = -1,  # TODO: refactor these mutables
        tree_number: int = -1,
        active: bool = False,
        connected: bool = False,
    ) -> None:
        self.node_type = node_type
        self.op_type = op_type
        self.split_type = split_type
        self.vertex = vertex
        self.comp_number = comp_number
        self.tree_number = tree_number
        self.num_marks = 0  # used only in refinement
        self.num_left_split_children = 0  # used only in refinement
        self.num_right_split_children = 0  # used only in refinement
        self.active = active
        self.connected = connected

    def copy(self) -> MDComputeNode:
        return MDComputeNode(
            self.node_type,
            self.op_type,
            self.split_type,
            self.vertex,
            self.comp_number,
            self.tree_number,
            self.active,
            self.connected,
        )

    @staticmethod
    def new_vertex_node(vertex: int): return MDComputeNode(NodeType.VERTEX, vertex=vertex)

    @staticmethod
    def new_operation_node(op_type: OperationType): return MDComputeNode(NodeType.OPERATION, op_type=op_type)

    @staticmethod
    def new_problem_node(connected: bool): return MDComputeNode(NodeType.PROBLEM, connected=connected)

    def is_vertex_node(self) -> bool: return self.node_type == NodeType.VERTEX
    def is_problem_node(self) -> bool: return self.node_type == NodeType.PROBLEM
    def is_operation_node(self) -> bool: return self.node_type == NodeType.OPERATION

    def add_mark(self) -> None:
        self.num_marks += 1

    def clear_marks(self) -> None:
        self.num_marks = 0

    def is_split_marked(self, split_type: SplitDirection):
        return self.split_type in [split_type, SplitDirection.MIXED]

    def set_split_mark(self, split_type: SplitDirection):
        assert split_type in [SplitDirection.LEFT, SplitDirection.RIGHT]

        if self.split_type == split_type:
            pass  # already set
        elif self.split_type == SplitDirection.NONE:
            self.split_type = split_type
        else:
            self.split_type = SplitDirection.MIXED

    def increment_num_split_children(self, split_type: SplitDirection) -> None:
        assert split_type in [SplitDirection.LEFT, SplitDirection.RIGHT]

        if split_type == SplitDirection.LEFT:
            self.num_left_split_children += 1
        else:
            self.num_right_split_children += 1

    def decrement_num_split_children(self, split_type: SplitDirection) -> None:
        assert split_type in [SplitDirection.LEFT, SplitDirection.RIGHT]

        if split_type == SplitDirection.LEFT:
            self.num_left_split_children -= 1
        else:
            self.num_right_split_children -= 1

    def get_num_split_children(self, split_type: SplitDirection) -> int:
        assert split_type in [SplitDirection.LEFT, SplitDirection.RIGHT]

        return self.num_left_split_children if split_type == SplitDirection.LEFT else self.num_right_split_children

    def clear_num_split_children(self) -> None:
        self.num_left_split_children = 0
        self.num_right_split_children = 0

    def clear(self) -> None:
        self.comp_number = -1
        self.tree_number = -1
        self.split_type = SplitDirection.NONE
        self.clear_num_split_children()

    def __str__(self) -> str:
        if self.node_type == NodeType.VERTEX:
            ret = str(self.vertex)
        elif self.node_type == NodeType.OPERATION:
            ret = {
                OperationType.PARALLEL: 'U',  # union
                OperationType.SERIES: 'J',  # join
                OperationType.PRIME: 'P',  # prime
            }[self.op_type]
        else:
            # problem
            return 'C' + ('-' if self.vertex < 0 else str(self.vertex))
        return ret

    def __repr__(self) -> str:
        ret = str(self)
        if self.node_type in [NodeType.VERTEX, NodeType.OPERATION]:
            ret += {
                SplitDirection.NONE: "-",
                SplitDirection.LEFT: "<",
                SplitDirection.RIGHT: ">",
                SplitDirection.MIXED: "+",
            }[self.split_type]
        return ret
