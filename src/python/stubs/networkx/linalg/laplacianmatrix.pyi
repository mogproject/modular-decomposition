from typing import Any

def laplacian_matrix(G, nodelist: Any | None = ..., weight: str = ...): ...
def normalized_laplacian_matrix(G, nodelist: Any | None = ..., weight: str = ...): ...
def directed_laplacian_matrix(
    G,
    nodelist: Any | None = ...,
    weight: str = ...,
    walk_type: Any | None = ...,
    alpha: float = ...,
): ...
def directed_combinatorial_laplacian_matrix(
    G,
    nodelist: Any | None = ...,
    weight: str = ...,
    walk_type: Any | None = ...,
    alpha: float = ...,
): ...
