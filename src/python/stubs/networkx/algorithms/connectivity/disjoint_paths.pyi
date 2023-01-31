from collections.abc import Generator
from typing import Any

from networkx.algorithms.flow import edmonds_karp

default_flow_func = edmonds_karp

def edge_disjoint_paths(
    G,
    s,
    t,
    flow_func: Any | None = ...,
    cutoff: Any | None = ...,
    auxiliary: Any | None = ...,
    residual: Any | None = ...,
) -> Generator[Any, None, None]: ...
def node_disjoint_paths(
    G,
    s,
    t,
    flow_func: Any | None = ...,
    cutoff: Any | None = ...,
    auxiliary: Any | None = ...,
    residual: Any | None = ...,
) -> Generator[Any, None, None]: ...
