#pragma once

#include "ds/graph/Graph.hpp"
#include "ds/set/FastSet.hpp"
#include "ds/tree/IntRootedForest.hpp"
#include "modular/compute/MDComputeNode.hpp"
#include "util/profiler.hpp"
#include "util/util.hpp"

namespace modular {
namespace compute {

typedef int VertexID;
typedef ds::tree::IntRootedForest<MDComputeNode> CompTree;

//================================================================================
//    Subroutines
//================================================================================
namespace impl {
typedef std::vector<bool> VB;
typedef std::vector<int> VI;
typedef std::vector<std::pair<int, int>> VII;
typedef std::vector<std::vector<VertexID>> VVV;

int compute(ds::graph::Graph const &graph, CompTree &tree, int main_prob, util::Profiler *prof = nullptr);

void process_neighbors(             //
    ds::graph::Graph const &graph,  //
    CompTree &tree,                 //
    VI alpha_list[],                //
    bool const visited[],           //
    VertexID pivot,                 //
    int current_prob,               //
    int nbr_prob                    //
);
int do_pivot(ds::graph::Graph const &graph,  //
             CompTree &tree,                 //
             VI alpha_list[],                //
             bool const visited[],           //
             int prob,                       //
             VertexID pivot                  //
);
int remove_extra_components(CompTree &tree, int prob);
void remove_layers(CompTree &tree, int prob);
void complete_alpha_lists(CompTree &tree, VI alpha_list[], ds::FastSet &vset, int prob);
void merge_components(CompTree &tree, int problem, int new_components);

void refine(CompTree &tree, VI const alpha_list[], int prob);
void promote(CompTree &tree, int prob);
void assemble(CompTree &tree, VI const *alpha_list, int prob);
}  // namespace impl

class MDSolver {
 public:
  static std::pair<CompTree, int> compute(ds::graph::Graph const &graph, util::Profiler *prof = nullptr) {
    // build computation tree
    CompTree tree;
    int n = graph.number_of_nodes();
    if (n == 0) return {tree, -1};

    // the first n nodes should be vertex nodes (cannot be removed)
    for (int i = 0; i < n; ++i) { tree.create_node(MDComputeNode::new_vertex_node(i)); }

    // create the main problem
    auto main_prob = tree.create_node(MDComputeNode::new_problem_node(false));

    // initially, all vertex nodes are the children of the main problem
    for (int i = n - 1; i >= 0; --i) tree.move_to(i, main_prob);

    // main logic
    auto new_root = impl::compute(graph, tree, main_prob, prof);

    // return result
    TRACE("result: %s", tree.to_string(new_root).c_str());
    return {tree, new_root};
  }

 private:
};
}  // namespace compute
}  // namespace modular
