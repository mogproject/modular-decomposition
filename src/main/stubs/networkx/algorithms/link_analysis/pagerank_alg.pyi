from typing import Any

def pagerank(
    G,
    alpha: float = ...,
    personalization: Any | None = ...,
    max_iter: int = ...,
    tol: float = ...,
    nstart: Any | None = ...,
    weight: str = ...,
    dangling: Any | None = ...,
): ...
def google_matrix(
    G,
    alpha: float = ...,
    personalization: Any | None = ...,
    nodelist: Any | None = ...,
    weight: str = ...,
    dangling: Any | None = ...,
): ...
def pagerank_numpy(
    G,
    alpha: float = ...,
    personalization: Any | None = ...,
    weight: str = ...,
    dangling: Any | None = ...,
): ...
def pagerank_scipy(
    G,
    alpha: float = ...,
    personalization: Any | None = ...,
    max_iter: int = ...,
    tol: float = ...,
    nstart: Any | None = ...,
    weight: str = ...,
    dangling: Any | None = ...,
): ...
