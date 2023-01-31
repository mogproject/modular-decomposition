from collections.abc import Generator
from typing import Any

def graph_edit_distance(
    G1,
    G2,
    node_match: Any | None = ...,
    edge_match: Any | None = ...,
    node_subst_cost: Any | None = ...,
    node_del_cost: Any | None = ...,
    node_ins_cost: Any | None = ...,
    edge_subst_cost: Any | None = ...,
    edge_del_cost: Any | None = ...,
    edge_ins_cost: Any | None = ...,
    roots: Any | None = ...,
    upper_bound: Any | None = ...,
    timeout: Any | None = ...,
): ...
def optimal_edit_paths(
    G1,
    G2,
    node_match: Any | None = ...,
    edge_match: Any | None = ...,
    node_subst_cost: Any | None = ...,
    node_del_cost: Any | None = ...,
    node_ins_cost: Any | None = ...,
    edge_subst_cost: Any | None = ...,
    edge_del_cost: Any | None = ...,
    edge_ins_cost: Any | None = ...,
    upper_bound: Any | None = ...,
): ...
def optimize_graph_edit_distance(
    G1,
    G2,
    node_match: Any | None = ...,
    edge_match: Any | None = ...,
    node_subst_cost: Any | None = ...,
    node_del_cost: Any | None = ...,
    node_ins_cost: Any | None = ...,
    edge_subst_cost: Any | None = ...,
    edge_del_cost: Any | None = ...,
    edge_ins_cost: Any | None = ...,
    upper_bound: Any | None = ...,
) -> Generator[Any, None, None]: ...
def optimize_edit_paths(
    G1,
    G2,
    node_match: Any | None = ...,
    edge_match: Any | None = ...,
    node_subst_cost: Any | None = ...,
    node_del_cost: Any | None = ...,
    node_ins_cost: Any | None = ...,
    edge_subst_cost: Any | None = ...,
    edge_del_cost: Any | None = ...,
    edge_ins_cost: Any | None = ...,
    upper_bound: Any | None = ...,
    strictly_decreasing: bool = ...,
    roots: Any | None = ...,
    timeout: Any | None = ...,
) -> Generator[Any, None, Any]: ...
def simrank_similarity(
    G,
    source: Any | None = ...,
    target: Any | None = ...,
    importance_factor: float = ...,
    max_iterations: int = ...,
    tolerance: float = ...,
): ...
def simrank_similarity_numpy(
    G,
    source: Any | None = ...,
    target: Any | None = ...,
    importance_factor: float = ...,
    max_iterations: int = ...,
    tolerance: float = ...,
): ...
def panther_similarity(
    G,
    source,
    k: int = ...,
    path_length: int = ...,
    c: float = ...,
    delta: float = ...,
    eps: Any | None = ...,
): ...
def generate_random_paths(
    G, sample_size, path_length: int = ..., index_map: Any | None = ...
) -> Generator[Any, None, None]: ...
