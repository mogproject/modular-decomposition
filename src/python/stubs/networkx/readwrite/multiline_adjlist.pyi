from collections.abc import Generator
from typing import Any

def generate_multiline_adjlist(
    G, delimiter: str = ...
) -> Generator[Any, None, None]: ...
def write_multiline_adjlist(
    G, path, delimiter: str = ..., comments: str = ..., encoding: str = ...
) -> None: ...
def parse_multiline_adjlist(
    lines,
    comments: str = ...,
    delimiter: Any | None = ...,
    create_using: Any | None = ...,
    nodetype: Any | None = ...,
    edgetype: Any | None = ...,
): ...
def read_multiline_adjlist(
    path,
    comments: str = ...,
    delimiter: Any | None = ...,
    create_using: Any | None = ...,
    nodetype: Any | None = ...,
    edgetype: Any | None = ...,
    encoding: str = ...,
): ...
