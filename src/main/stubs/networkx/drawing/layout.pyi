from typing import Any

def random_layout(
    G, center: Any | None = ..., dim: int = ..., seed: Any | None = ...
): ...
def circular_layout(G, scale: int = ..., center: Any | None = ..., dim: int = ...): ...
def shell_layout(
    G,
    nlist: Any | None = ...,
    rotate: Any | None = ...,
    scale: int = ...,
    center: Any | None = ...,
    dim: int = ...,
): ...
def bipartite_layout(
    G,
    nodes,
    align: str = ...,
    scale: int = ...,
    center: Any | None = ...,
    aspect_ratio=...,
): ...
def spring_layout(
    G,
    k: Any | None = ...,
    pos: Any | None = ...,
    fixed: Any | None = ...,
    iterations: int = ...,
    threshold: float = ...,
    weight: str = ...,
    scale: int = ...,
    center: Any | None = ...,
    dim: int = ...,
    seed: Any | None = ...,
): ...

fruchterman_reingold_layout = spring_layout

def kamada_kawai_layout(
    G,
    dist: Any | None = ...,
    pos: Any | None = ...,
    weight: str = ...,
    scale: int = ...,
    center: Any | None = ...,
    dim: int = ...,
): ...
def spectral_layout(
    G, weight: str = ..., scale: int = ..., center: Any | None = ..., dim: int = ...
): ...
def planar_layout(G, scale: int = ..., center: Any | None = ..., dim: int = ...): ...
def spiral_layout(
    G,
    scale: int = ...,
    center: Any | None = ...,
    dim: int = ...,
    resolution: float = ...,
    equidistant: bool = ...,
): ...
def multipartite_layout(
    G,
    subset_key: str = ...,
    align: str = ...,
    scale: int = ...,
    center: Any | None = ...,
): ...
def rescale_layout(pos, scale: int = ...): ...
def rescale_layout_dict(pos, scale: int = ...): ...
