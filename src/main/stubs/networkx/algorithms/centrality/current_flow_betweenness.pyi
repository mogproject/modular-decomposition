from typing import Any

def approximate_current_flow_betweenness_centrality(
    G,
    normalized: bool = ...,
    weight: Any | None = ...,
    dtype=...,
    solver: str = ...,
    epsilon: float = ...,
    kmax: int = ...,
    seed: Any | None = ...,
): ...
def current_flow_betweenness_centrality(
    G, normalized: bool = ..., weight: Any | None = ..., dtype=..., solver: str = ...
): ...
def edge_current_flow_betweenness_centrality(
    G, normalized: bool = ..., weight: Any | None = ..., dtype=..., solver: str = ...
): ...
