from typing import Any

def to_pandas_adjacency(
    G,
    nodelist: Any | None = ...,
    dtype: Any | None = ...,
    order: Any | None = ...,
    multigraph_weight=...,
    weight: str = ...,
    nonedge: float = ...,
): ...
def from_pandas_adjacency(df, create_using: Any | None = ...): ...
def to_pandas_edgelist(
    G,
    source: str = ...,
    target: str = ...,
    nodelist: Any | None = ...,
    dtype: Any | None = ...,
    order: Any | None = ...,
    edge_key: Any | None = ...,
): ...
def from_pandas_edgelist(
    df,
    source: str = ...,
    target: str = ...,
    edge_attr: Any | None = ...,
    create_using: Any | None = ...,
    edge_key: Any | None = ...,
): ...
def to_numpy_matrix(
    G,
    nodelist: Any | None = ...,
    dtype: Any | None = ...,
    order: Any | None = ...,
    multigraph_weight=...,
    weight: str = ...,
    nonedge: float = ...,
): ...
def from_numpy_matrix(
    A, parallel_edges: bool = ..., create_using: Any | None = ...
): ...
def to_numpy_recarray(
    G, nodelist: Any | None = ..., dtype: Any | None = ..., order: Any | None = ...
): ...
def to_scipy_sparse_array(
    G,
    nodelist: Any | None = ...,
    dtype: Any | None = ...,
    weight: str = ...,
    format: str = ...,
): ...
def to_scipy_sparse_matrix(
    G,
    nodelist: Any | None = ...,
    dtype: Any | None = ...,
    weight: str = ...,
    format: str = ...,
): ...
def from_scipy_sparse_matrix(
    A,
    parallel_edges: bool = ...,
    create_using: Any | None = ...,
    edge_attribute: str = ...,
): ...
def from_scipy_sparse_array(
    A,
    parallel_edges: bool = ...,
    create_using: Any | None = ...,
    edge_attribute: str = ...,
): ...
def to_numpy_array(
    G,
    nodelist: Any | None = ...,
    dtype: Any | None = ...,
    order: Any | None = ...,
    multigraph_weight=...,
    weight: str = ...,
    nonedge: float = ...,
): ...
def from_numpy_array(A, parallel_edges: bool = ..., create_using: Any | None = ...): ...
