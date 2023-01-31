from enum import Enum
from typing import Any

class EdgePartition(Enum):
    OPEN: int
    INCLUDED: int
    EXCLUDED: int

def minimum_spanning_edges(
    G,
    algorithm: str = ...,
    weight: str = ...,
    keys: bool = ...,
    data: bool = ...,
    ignore_nan: bool = ...,
): ...
def maximum_spanning_edges(
    G,
    algorithm: str = ...,
    weight: str = ...,
    keys: bool = ...,
    data: bool = ...,
    ignore_nan: bool = ...,
): ...
def minimum_spanning_tree(
    G, weight: str = ..., algorithm: str = ..., ignore_nan: bool = ...
): ...
def partition_spanning_tree(
    G,
    minimum: bool = ...,
    weight: str = ...,
    partition: str = ...,
    ignore_nan: bool = ...,
): ...
def maximum_spanning_tree(
    G, weight: str = ..., algorithm: str = ..., ignore_nan: bool = ...
): ...

class SpanningTreeIterator:
    class Partition:
        mst_weight: float
        partition_dict: dict
        def __copy__(self): ...
        def __init__(self, mst_weight, partition_dict) -> None: ...
        def __lt__(self, other): ...
        def __gt__(self, other): ...
        def __le__(self, other): ...
        def __ge__(self, other): ...
    G: Any
    weight: Any
    minimum: Any
    ignore_nan: Any
    partition_key: str
    def __init__(
        self, G, weight: str = ..., minimum: bool = ..., ignore_nan: bool = ...
    ) -> None: ...
    partition_queue: Any
    def __iter__(self): ...
    def __next__(self): ...
