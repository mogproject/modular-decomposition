from collections.abc import Generator
from typing import Any

def write_edgelist(
    G,
    path,
    comments: str = ...,
    delimiter: str = ...,
    data: bool = ...,
    encoding: str = ...,
) -> None: ...
def generate_edgelist(
    G, delimiter: str = ..., data: bool = ...
) -> Generator[Any, None, None]: ...
def parse_edgelist(
    lines,
    comments: str = ...,
    delimiter: Any | None = ...,
    create_using: Any | None = ...,
    nodetype: Any | None = ...,
    data: bool = ...,
): ...
def read_edgelist(
    path,
    comments: str = ...,
    delimiter: Any | None = ...,
    create_using: Any | None = ...,
    nodetype: Any | None = ...,
    data: bool = ...,
    edgetype: Any | None = ...,
    encoding: str = ...,
): ...
