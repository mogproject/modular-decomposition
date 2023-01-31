from collections.abc import Generator
from typing import Any

def write_graphml_xml(
    G,
    path,
    encoding: str = ...,
    prettyprint: bool = ...,
    infer_numeric_types: bool = ...,
    named_key_ids: bool = ...,
    edge_id_from_attribute: Any | None = ...,
) -> None: ...
def write_graphml_lxml(
    G,
    path,
    encoding: str = ...,
    prettyprint: bool = ...,
    infer_numeric_types: bool = ...,
    named_key_ids: bool = ...,
    edge_id_from_attribute: Any | None = ...,
): ...
def generate_graphml(
    G,
    encoding: str = ...,
    prettyprint: bool = ...,
    named_key_ids: bool = ...,
    edge_id_from_attribute: Any | None = ...,
) -> None: ...
def read_graphml(
    path, node_type=..., edge_key_type=..., force_multigraph: bool = ...
): ...
def parse_graphml(
    graphml_string, node_type=..., edge_key_type=..., force_multigraph: bool = ...
): ...

class GraphML:
    NS_GRAPHML: str
    NS_XSI: str
    NS_Y: str
    SCHEMALOCATION: Any
    xml_type: Any
    python_type: Any
    def construct_types(self) -> None: ...
    convert_bool: Any
    def get_xml_type(self, key): ...

class GraphMLWriter(GraphML):
    myElement: Any
    infer_numeric_types: Any
    prettyprint: Any
    named_key_ids: Any
    edge_id_from_attribute: Any
    encoding: Any
    xml: Any
    keys: Any
    attributes: Any
    attribute_types: Any
    def __init__(
        self,
        graph: Any | None = ...,
        encoding: str = ...,
        prettyprint: bool = ...,
        infer_numeric_types: bool = ...,
        named_key_ids: bool = ...,
        edge_id_from_attribute: Any | None = ...,
    ) -> None: ...
    def attr_type(self, name, scope, value): ...
    def get_key(self, name, attr_type, scope, default): ...
    def add_data(
        self, name, element_type, value, scope: str = ..., default: Any | None = ...
    ): ...
    def add_attributes(self, scope, xml_obj, data, default) -> None: ...
    def add_nodes(self, G, graph_element) -> None: ...
    def add_edges(self, G, graph_element) -> None: ...
    def add_graph_element(self, G) -> None: ...
    def add_graphs(self, graph_list) -> None: ...
    def dump(self, stream) -> None: ...
    def indent(self, elem, level: int = ...) -> None: ...

class IncrementalElement:
    xml: Any
    prettyprint: Any
    def __init__(self, xml, prettyprint) -> None: ...
    def append(self, element) -> None: ...

class GraphMLWriterLxml(GraphMLWriter):
    myElement: Any
    named_key_ids: Any
    edge_id_from_attribute: Any
    infer_numeric_types: Any
    xml: Any
    keys: Any
    attribute_types: Any
    def __init__(
        self,
        path,
        graph: Any | None = ...,
        encoding: str = ...,
        prettyprint: bool = ...,
        infer_numeric_types: bool = ...,
        named_key_ids: bool = ...,
        edge_id_from_attribute: Any | None = ...,
    ) -> None: ...
    def add_graph_element(self, G) -> None: ...
    def add_attributes(self, scope, xml_obj, data, default) -> None: ...
    def dump(self) -> None: ...

write_graphml = write_graphml_lxml

class GraphMLReader(GraphML):
    node_type: Any
    edge_key_type: Any
    multigraph: Any
    edge_ids: Any
    def __init__(
        self, node_type=..., edge_key_type=..., force_multigraph: bool = ...
    ) -> None: ...
    xml: Any
    def __call__(
        self, path: Any | None = ..., string: Any | None = ...
    ) -> Generator[Any, None, None]: ...
    def make_graph(self, graph_xml, graphml_keys, defaults, G: Any | None = ...): ...
    def add_node(self, G, node_xml, graphml_keys, defaults) -> None: ...
    def add_edge(self, G, edge_element, graphml_keys) -> None: ...
    def decode_data_elements(self, graphml_keys, obj_xml): ...
    def find_graphml_keys(self, graph_element): ...
