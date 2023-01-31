from typing import Any

def hits(
    G,
    max_iter: int = ...,
    tol: float = ...,
    nstart: Any | None = ...,
    normalized: bool = ...,
): ...
def authority_matrix(G, nodelist: Any | None = ...): ...
def hub_matrix(G, nodelist: Any | None = ...): ...
def hits_numpy(G, normalized: bool = ...): ...
def hits_scipy(
    G,
    max_iter: int = ...,
    tol: float = ...,
    nstart: Any | None = ...,
    normalized: bool = ...,
): ...
