from typing import Any

from .digraph import DiGraph
from .graph import Graph
from .multidigraph import MultiDiGraph
from .multigraph import MultiGraph

class OrderedGraph(Graph):
    node_dict_factory: Any
    adjlist_outer_dict_factory: Any
    adjlist_inner_dict_factory: Any
    edge_attr_dict_factory: Any
    def __init__(self, incoming_graph_data: Any | None = ..., **attr) -> None: ...

class OrderedDiGraph(DiGraph):
    node_dict_factory: Any
    adjlist_outer_dict_factory: Any
    adjlist_inner_dict_factory: Any
    edge_attr_dict_factory: Any
    def __init__(self, incoming_graph_data: Any | None = ..., **attr) -> None: ...

class OrderedMultiGraph(MultiGraph):
    node_dict_factory: Any
    adjlist_outer_dict_factory: Any
    adjlist_inner_dict_factory: Any
    edge_key_dict_factory: Any
    edge_attr_dict_factory: Any
    def __init__(self, incoming_graph_data: Any | None = ..., **attr) -> None: ...

class OrderedMultiDiGraph(MultiDiGraph):
    node_dict_factory: Any
    adjlist_outer_dict_factory: Any
    adjlist_inner_dict_factory: Any
    edge_key_dict_factory: Any
    edge_attr_dict_factory: Any
    def __init__(self, incoming_graph_data: Any | None = ..., **attr) -> None: ...
