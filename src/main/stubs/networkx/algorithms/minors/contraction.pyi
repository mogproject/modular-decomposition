from typing import Any

def equivalence_classes(iterable, relation): ...
def quotient_graph(
    G,
    partition,
    edge_relation: Any | None = ...,
    node_data: Any | None = ...,
    edge_data: Any | None = ...,
    relabel: bool = ...,
    create_using: Any | None = ...,
): ...
def contracted_nodes(G, u, v, self_loops: bool = ..., copy: bool = ...): ...

identified_nodes = contracted_nodes

def contracted_edge(G, edge, self_loops: bool = ..., copy: bool = ...): ...
