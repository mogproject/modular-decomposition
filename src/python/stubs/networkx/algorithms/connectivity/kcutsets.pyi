from collections.abc import Generator
from typing import Any

from networkx.algorithms.flow import edmonds_karp

default_flow_func = edmonds_karp

def all_node_cuts(
    G, k: Any | None = ..., flow_func: Any | None = ...
) -> Generator[Any, None, None]: ...
