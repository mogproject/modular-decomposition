from typing import Any

def attribute_mixing_dict(
    G, attribute, nodes: Any | None = ..., normalized: bool = ...
): ...
def attribute_mixing_matrix(
    G,
    attribute,
    nodes: Any | None = ...,
    mapping: Any | None = ...,
    normalized: bool = ...,
): ...
def degree_mixing_dict(
    G,
    x: str = ...,
    y: str = ...,
    weight: Any | None = ...,
    nodes: Any | None = ...,
    normalized: bool = ...,
): ...
def degree_mixing_matrix(
    G,
    x: str = ...,
    y: str = ...,
    weight: Any | None = ...,
    nodes: Any | None = ...,
    normalized: bool = ...,
    mapping: Any | None = ...,
): ...
def numeric_mixing_matrix(
    G,
    attribute,
    nodes: Any | None = ...,
    normalized: bool = ...,
    mapping: Any | None = ...,
): ...
def mixing_dict(xy, normalized: bool = ...): ...
