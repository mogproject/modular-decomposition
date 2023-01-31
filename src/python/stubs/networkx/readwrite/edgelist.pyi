from collections.abc import Generator
from typing import Any

def generate_edgelist(
    G, delimiter: str = ..., data: bool = ...
) -> Generator[Any, None, None]: ...
def write_edgelist(
    G,
    path,
    comments: str = ...,
    delimiter: str = ...,
    data: bool = ...,
    encoding: str = ...,
) -> None: ...
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
def write_weighted_edgelist(
    G, path, comments: str = ..., delimiter: str = ..., encoding: str = ...
) -> None: ...
def read_weighted_edgelist(
    path,
    comments: str = ...,
    delimiter: Any | None = ...,
    create_using: Any | None = ...,
    nodetype: Any | None = ...,
    encoding: str = ...,
): ...
