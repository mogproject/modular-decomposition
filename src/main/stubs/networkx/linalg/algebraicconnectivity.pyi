from typing import Any

class _PCGSolver:
    def __init__(self, A, M) -> None: ...
    def solve(self, B, tol): ...

class _LUSolver:
    def __init__(self, A) -> None: ...
    def solve(self, B, tol: Any | None = ...): ...

def algebraic_connectivity(
    G,
    weight: str = ...,
    normalized: bool = ...,
    tol: float = ...,
    method: str = ...,
    seed: Any | None = ...,
): ...
def fiedler_vector(
    G,
    weight: str = ...,
    normalized: bool = ...,
    tol: float = ...,
    method: str = ...,
    seed: Any | None = ...,
): ...
def spectral_ordering(
    G,
    weight: str = ...,
    normalized: bool = ...,
    tol: float = ...,
    method: str = ...,
    seed: Any | None = ...,
): ...
