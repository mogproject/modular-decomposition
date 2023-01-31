from typing import Any

from .preflowpush import preflow_push

default_flow_func = preflow_push

def maximum_flow(
    flowG, _s, _t, capacity: str = ..., flow_func: Any | None = ..., **kwargs
): ...
def maximum_flow_value(
    flowG, _s, _t, capacity: str = ..., flow_func: Any | None = ..., **kwargs
): ...
def minimum_cut(
    flowG, _s, _t, capacity: str = ..., flow_func: Any | None = ..., **kwargs
): ...
def minimum_cut_value(
    flowG, _s, _t, capacity: str = ..., flow_func: Any | None = ..., **kwargs
): ...
