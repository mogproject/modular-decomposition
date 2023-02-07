#include <iostream>

#include "modular/MDTree.hpp"
#include "readwrite/edge_list.hpp"

using namespace std;

int main(int argc, char* argv[]) {
  // load graph
  auto graph = readwrite::read_edge_list(std::cin);

  // run algorithm
  auto result = modular::modular_decomposition_time(graph, true);

  // output result
  printf("%d\n", result.first.modular_width());
  printf("%.10f\n", result.second);
  printf("%s\n", result.first.to_string().c_str());

  return 0;
}
