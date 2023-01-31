from collections.abc import Generator
from typing import Any

def generate_adjlist(G, delimiter: str = ...) -> Generator[Any, None, None]: ...
def write_adjlist(
    G, path, comments: str = ..., delimiter: str = ..., encoding: str = ...
) -> None: ...
def parse_adjlist(
    lines,
    comments: str = ...,
    delimiter: Any | None = ...,
    create_using: Any | None = ...,
    nodetype: Any | None = ...,
): ...
def read_adjlist(
    path,
    comments: str = ...,
    delimiter: Any | None = ...,
    create_using: Any | None = ...,
    nodetype: Any | None = ...,
    encoding: str = ...,
): ...
