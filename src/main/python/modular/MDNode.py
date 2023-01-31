from typing import Any, Optional

from modular.OperationType import OperationType

VertexId = Any


class MDNode:
    def __init__(
        self,
        vertex: Optional[VertexId] = None,
        op: Optional[OperationType] = None,
        vertices_begin: int = -1,
        vertices_end: int = -1
    ) -> None:
        self.vertex = vertex
        self.op = op
        self.vertices_begin = vertices_begin
        self.vertices_end = vertices_end

    def is_vertex_node(self) -> bool: return self.vertex is not None
    def is_operation_node(self) -> bool: return self.vertex is None
    def is_prime_node(self) -> bool: return self.op == OperationType.PRIME
    def is_join_node(self) -> bool: return self.op == OperationType.SERIES
    def is_union_node(self) -> bool: return self.op == OperationType.PARALLEL

    def size(self) -> int: return self.vertices_end - self.vertices_begin

    def __repr__(self) -> str:
        if self.is_vertex_node():
            return repr(self.vertex)
        return {OperationType.PRIME: 'P', OperationType.SERIES: 'J', OperationType.PARALLEL: 'U', None: '-'}[self.op]
