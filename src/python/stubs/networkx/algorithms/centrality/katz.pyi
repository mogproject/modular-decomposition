from typing import Any

def katz_centrality(
    G,
    alpha: float = ...,
    beta: float = ...,
    max_iter: int = ...,
    tol: float = ...,
    nstart: Any | None = ...,
    normalized: bool = ...,
    weight: Any | None = ...,
): ...
def katz_centrality_numpy(
    G,
    alpha: float = ...,
    beta: float = ...,
    normalized: bool = ...,
    weight: Any | None = ...,
): ...
