from collections.abc import Generator
from typing import Any

def bfs_edges(
    G,
    source,
    reverse: bool = ...,
    depth_limit: Any | None = ...,
    sort_neighbors: Any | None = ...,
) -> None: ...
def bfs_tree(
    G,
    source,
    reverse: bool = ...,
    depth_limit: Any | None = ...,
    sort_neighbors: Any | None = ...,
): ...
def bfs_predecessors(
    G, source, depth_limit: Any | None = ..., sort_neighbors: Any | None = ...
) -> Generator[Any, None, None]: ...
def bfs_successors(
    G, source, depth_limit: Any | None = ..., sort_neighbors: Any | None = ...
) -> Generator[Any, None, None]: ...
def descendants_at_distance(G, source, distance): ...
