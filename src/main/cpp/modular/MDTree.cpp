#include <chrono>

#include "MDTree.hpp"

namespace modular {

std::ostream &operator<<(std::ostream &os, MDNode const &node) { return os << node.to_string(); }

MDTree modular_decomposition(ds::graph::Graph const &graph, bool sorted) { return MDTree(graph, sorted); }

std::pair<MDTree, double> modular_decomposition_time(ds::graph::Graph const &graph, bool sorted, util::Profiler *prof) {
  auto time_start = std::chrono::system_clock::now();
  auto ret = MDTree(graph, false, prof);
  auto time_finish = std::chrono::system_clock::now();
  auto elapsed = time_finish - time_start;
  auto elapsed_sec = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() * 1e-9;

  if (sorted) ret.sort();
  return {ret, elapsed_sec};
}
}  // namespace modular
