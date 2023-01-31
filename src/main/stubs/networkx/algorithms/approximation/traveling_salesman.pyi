from typing import Any

def christofides(G, weight: str = ..., tree: Any | None = ...): ...
def traveling_salesman_problem(
    G,
    weight: str = ...,
    nodes: Any | None = ...,
    cycle: bool = ...,
    method: Any | None = ...,
): ...
def asadpour_atsp(
    G, weight: str = ..., seed: Any | None = ..., source: Any | None = ...
): ...
def greedy_tsp(G, weight: str = ..., source: Any | None = ...): ...
def simulated_annealing_tsp(
    G,
    init_cycle,
    weight: str = ...,
    source: Any | None = ...,
    temp: int = ...,
    move: str = ...,
    max_iterations: int = ...,
    N_inner: int = ...,
    alpha: float = ...,
    seed: Any | None = ...,
): ...
def threshold_accepting_tsp(
    G,
    init_cycle,
    weight: str = ...,
    source: Any | None = ...,
    threshold: int = ...,
    move: str = ...,
    max_iterations: int = ...,
    N_inner: int = ...,
    alpha: float = ...,
    seed: Any | None = ...,
): ...
