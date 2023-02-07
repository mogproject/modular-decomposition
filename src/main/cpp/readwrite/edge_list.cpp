#include <map>

#include "ds/graph/Graph.hpp"
#include "edge_list.hpp"

namespace readwrite {
ds::graph::Graph read_edge_list(std::istream &is) {
  std::vector<std::pair<int, int>> edges;
  std::vector<int> loops;
  int max_label = -1;

  for (std::string line; std::getline(is, line);) {
    if (line.empty()) continue;

    auto ss = std::stringstream(line);
    int u, v;
    ss >> u >> v;
    max_label = std::max(max_label, u);
    max_label = std::max(max_label, v);
    edges.push_back({u, v});
  }

  int n = max_label + 1;
  return ds::graph::Graph(n, edges);
}

ds::graph::Graph load_edge_list(char const *path) {
  std::ifstream f(path);
  if (f.fail()) throw std::invalid_argument(util::format("Failed to open file: %s", path));
  return read_edge_list(f);
}
}  // namespace readwrite
