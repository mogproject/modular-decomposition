from typing import Any

from .edmondskarp import edmonds_karp

default_flow_func = edmonds_karp

def gomory_hu_tree(G, capacity: str = ..., flow_func: Any | None = ...): ...
