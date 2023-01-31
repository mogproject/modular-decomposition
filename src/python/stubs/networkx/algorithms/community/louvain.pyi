from collections.abc import Generator
from typing import Any

def louvain_communities(
    G,
    weight: str = ...,
    resolution: int = ...,
    threshold: float = ...,
    seed: Any | None = ...,
): ...
def louvain_partitions(
    G,
    weight: str = ...,
    resolution: int = ...,
    threshold: float = ...,
    seed: Any | None = ...,
) -> Generator[Any, None, None]: ...
