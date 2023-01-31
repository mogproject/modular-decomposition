from typing import Any

def draw(G, pos: Any | None = ..., ax: Any | None = ..., **kwds) -> None: ...
def draw_networkx(
    G, pos: Any | None = ..., arrows: Any | None = ..., with_labels: bool = ..., **kwds
) -> None: ...
def draw_networkx_nodes(
    G,
    pos,
    nodelist: Any | None = ...,
    node_size: int = ...,
    node_color: str = ...,
    node_shape: str = ...,
    alpha: Any | None = ...,
    cmap: Any | None = ...,
    vmin: Any | None = ...,
    vmax: Any | None = ...,
    ax: Any | None = ...,
    linewidths: Any | None = ...,
    edgecolors: Any | None = ...,
    label: Any | None = ...,
    margins: Any | None = ...,
): ...
def draw_networkx_edges(
    G,
    pos,
    edgelist: Any | None = ...,
    width: float = ...,
    edge_color: str = ...,
    style: str = ...,
    alpha: Any | None = ...,
    arrowstyle: str = ...,
    arrowsize: int = ...,
    edge_cmap: Any | None = ...,
    edge_vmin: Any | None = ...,
    edge_vmax: Any | None = ...,
    ax: Any | None = ...,
    arrows: Any | None = ...,
    label: Any | None = ...,
    node_size: int = ...,
    nodelist: Any | None = ...,
    node_shape: str = ...,
    connectionstyle: str = ...,
    min_source_margin: int = ...,
    min_target_margin: int = ...,
): ...
def draw_networkx_labels(
    G,
    pos,
    labels: Any | None = ...,
    font_size: int = ...,
    font_color: str = ...,
    font_family: str = ...,
    font_weight: str = ...,
    alpha: Any | None = ...,
    bbox: Any | None = ...,
    horizontalalignment: str = ...,
    verticalalignment: str = ...,
    ax: Any | None = ...,
    clip_on: bool = ...,
): ...
def draw_networkx_edge_labels(
    G,
    pos,
    edge_labels: Any | None = ...,
    label_pos: float = ...,
    font_size: int = ...,
    font_color: str = ...,
    font_family: str = ...,
    font_weight: str = ...,
    alpha: Any | None = ...,
    bbox: Any | None = ...,
    horizontalalignment: str = ...,
    verticalalignment: str = ...,
    ax: Any | None = ...,
    rotate: bool = ...,
    clip_on: bool = ...,
): ...
def draw_circular(G, **kwargs) -> None: ...
def draw_kamada_kawai(G, **kwargs) -> None: ...
def draw_random(G, **kwargs) -> None: ...
def draw_spectral(G, **kwargs) -> None: ...
def draw_spring(G, **kwargs) -> None: ...
def draw_shell(G, nlist: Any | None = ..., **kwargs) -> None: ...
def draw_planar(G, **kwargs) -> None: ...
