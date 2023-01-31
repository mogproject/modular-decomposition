from typing import Any

from networkx.algorithms.flow import edmonds_karp

default_flow_func = edmonds_karp

def minimum_st_edge_cut(
    G,
    s,
    t,
    flow_func: Any | None = ...,
    auxiliary: Any | None = ...,
    residual: Any | None = ...,
): ...
def minimum_st_node_cut(
    G,
    s,
    t,
    flow_func: Any | None = ...,
    auxiliary: Any | None = ...,
    residual: Any | None = ...,
): ...
def minimum_node_cut(
    G, s: Any | None = ..., t: Any | None = ..., flow_func: Any | None = ...
): ...
def minimum_edge_cut(
    G, s: Any | None = ..., t: Any | None = ..., flow_func: Any | None = ...
): ...
