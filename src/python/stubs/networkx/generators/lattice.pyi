from typing import Any

def grid_2d_graph(m, n, periodic: bool = ..., create_using: Any | None = ...): ...
def grid_graph(dim, periodic: bool = ...): ...
def hypercube_graph(n): ...
def triangular_lattice_graph(
    m,
    n,
    periodic: bool = ...,
    with_positions: bool = ...,
    create_using: Any | None = ...,
): ...
def hexagonal_lattice_graph(
    m,
    n,
    periodic: bool = ...,
    with_positions: bool = ...,
    create_using: Any | None = ...,
): ...
