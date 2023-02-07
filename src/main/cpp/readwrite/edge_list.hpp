#pragma once
#include <fstream>
#include <sstream>

namespace readwrite {
ds::graph::Graph read_edge_list(std::istream &is);

ds::graph::Graph load_edge_list(char const *path);
}  // namespace readwrite
