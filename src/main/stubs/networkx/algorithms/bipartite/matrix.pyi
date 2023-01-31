from typing import Any

def biadjacency_matrix(
    G,
    row_order,
    column_order: Any | None = ...,
    dtype: Any | None = ...,
    weight: str = ...,
    format: str = ...,
): ...
def from_biadjacency_matrix(
    A, create_using: Any | None = ..., edge_attribute: str = ...
): ...
