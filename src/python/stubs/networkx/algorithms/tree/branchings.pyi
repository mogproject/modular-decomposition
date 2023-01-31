from typing import Any

import networkx as nx

def branching_weight(G, attr: str = ..., default: int = ...): ...
def greedy_branching(
    G, attr: str = ..., default: int = ..., kind: str = ..., seed: Any | None = ...
): ...

class MultiDiGraph_EdgeKey(nx.MultiDiGraph):
    edge_index: Any
    def __init__(self, incoming_graph_data: Any | None = ..., **attr) -> None: ...
    def remove_node(self, n) -> None: ...
    def remove_nodes_from(self, nbunch) -> None: ...
    def add_edge(self, u_for_edge, v_for_edge, key_for_edge, **attr) -> None: ...
    def add_edges_from(self, ebunch_to_add, **attr) -> None: ...
    def remove_edge_with_key(self, key) -> None: ...
    def remove_edges_from(self, ebunch) -> None: ...

class Edmonds:
    G_original: Any
    store: bool
    edges: Any
    template: Any
    def __init__(self, G, seed: Any | None = ...) -> None: ...
    def find_optimum(
        self,
        attr: str = ...,
        default: int = ...,
        kind: str = ...,
        style: str = ...,
        preserve_attrs: bool = ...,
        partition: Any | None = ...,
        seed: Any | None = ...,
    ): ...

def maximum_branching(
    G,
    attr: str = ...,
    default: int = ...,
    preserve_attrs: bool = ...,
    partition: Any | None = ...,
): ...
def minimum_branching(
    G,
    attr: str = ...,
    default: int = ...,
    preserve_attrs: bool = ...,
    partition: Any | None = ...,
): ...
def maximum_spanning_arborescence(
    G,
    attr: str = ...,
    default: int = ...,
    preserve_attrs: bool = ...,
    partition: Any | None = ...,
): ...
def minimum_spanning_arborescence(
    G,
    attr: str = ...,
    default: int = ...,
    preserve_attrs: bool = ...,
    partition: Any | None = ...,
): ...

class ArborescenceIterator:
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
    method: Any
    partition_key: str
    init_partition: Any
    def __init__(
        self,
        G,
        weight: str = ...,
        minimum: bool = ...,
        init_partition: Any | None = ...,
    ) -> None: ...
    partition_queue: Any
    def __iter__(self): ...
    def __next__(self): ...
