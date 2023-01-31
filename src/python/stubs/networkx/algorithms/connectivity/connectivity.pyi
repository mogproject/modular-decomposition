from typing import Any

from networkx.algorithms.flow import edmonds_karp

default_flow_func = edmonds_karp

def local_node_connectivity(
    G,
    s,
    t,
    flow_func: Any | None = ...,
    auxiliary: Any | None = ...,
    residual: Any | None = ...,
    cutoff: Any | None = ...,
): ...
def node_connectivity(
    G, s: Any | None = ..., t: Any | None = ..., flow_func: Any | None = ...
): ...
def average_node_connectivity(G, flow_func: Any | None = ...): ...
def all_pairs_node_connectivity(
    G, nbunch: Any | None = ..., flow_func: Any | None = ...
): ...
def local_edge_connectivity(
    G,
    s,
    t,
    flow_func: Any | None = ...,
    auxiliary: Any | None = ...,
    residual: Any | None = ...,
    cutoff: Any | None = ...,
): ...
def edge_connectivity(
    G,
    s: Any | None = ...,
    t: Any | None = ...,
    flow_func: Any | None = ...,
    cutoff: Any | None = ...,
): ...
