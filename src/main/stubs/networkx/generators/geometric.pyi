from typing import Any

def geometric_edges(G, radius, p): ...
def random_geometric_graph(
    n,
    radius,
    dim: int = ...,
    pos: Any | None = ...,
    p: int = ...,
    seed: Any | None = ...,
): ...
def soft_random_geometric_graph(
    n,
    radius,
    dim: int = ...,
    pos: Any | None = ...,
    p: int = ...,
    p_dist: Any | None = ...,
    seed: Any | None = ...,
): ...
def geographical_threshold_graph(
    n,
    theta,
    dim: int = ...,
    pos: Any | None = ...,
    weight: Any | None = ...,
    metric: Any | None = ...,
    p_dist: Any | None = ...,
    seed: Any | None = ...,
): ...
def waxman_graph(
    n,
    beta: float = ...,
    alpha: float = ...,
    L: Any | None = ...,
    domain=...,
    metric: Any | None = ...,
    seed: Any | None = ...,
): ...
def navigable_small_world_graph(
    n, p: int = ..., q: int = ..., r: int = ..., dim: int = ..., seed: Any | None = ...
): ...
def thresholded_random_geometric_graph(
    n,
    radius,
    theta,
    dim: int = ...,
    pos: Any | None = ...,
    weight: Any | None = ...,
    p: int = ...,
    seed: Any | None = ...,
): ...
