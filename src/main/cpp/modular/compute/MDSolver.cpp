#include "MDSolver.hpp"

namespace modular {
namespace compute {
namespace impl {

int compute(ds::graph::Graph const &graph, CompTree &tree, int main_prob, util::Profiler *prof) {
  TRACE("start compute(): %s", tree.to_string(main_prob).c_str());
  PROF(util::pstart(prof, "compute()"));

  int n = graph.number_of_nodes();
  int current_prob = main_prob;

  VVV alpha_list(n);
  VVV fp_neighbors(n);  // used only for assembly -> compute_fact_perm_edges()
  bool visited[n];
  for (int i = 0; i < n; ++i) {
    visited[i] = false;
  }
  ds::FastSet vset(n);
  int result = -1;
  int t = 0;

  while (tree.is_valid(current_prob)) {
    ++t;
    TRACE("main problem (%d): %s", t, tree.to_string(tree.get_root(current_prob)).c_str());
    TRACE("current problem: %s", tree.to_string(current_prob).c_str());
    for (int i = 0; i < n; ++i) {
      TRACE("visited [%d]: %s", i, visited[i] ? "True" : "False");
      TRACE("alpha [%d]: %s", i, util::to_string(alpha_list[i]).c_str());
    }

    auto &cp = tree[current_prob];
    cp.data.active = true;

    auto &fc = tree[cp.first_child];

    if (!fc.data.is_problem_node()) {
      // first, needs to solve subproblems
      visited[cp.first_child] = true;  // set visited

      if (cp.has_only_one_child()) {
        // base case
        PROF(util::pcount(prof, "solve(): base case"));
        PROF(util::pstart(prof, "process_neighbors()"));
        process_neighbors(graph, tree, alpha_list, visited, cp.first_child, current_prob, -1);
        PROF(util::pstop(prof, "process_neighbors()"));
      } else {
        // pivot at the first child
        PROF(util::pstart(prof, "do_pivot()"));
        auto pivoted = do_pivot(graph, tree, alpha_list, visited, current_prob, cp.first_child);
        PROF(util::pstop(prof, "do_pivot()"));

        // dig into the first subproblem
        current_prob = tree[pivoted].first_child;
        continue;
      }
    } else {
      // now, ready to compute this problem
      PROF(util::pstart(prof, "remove_extra_components()"));
      auto extra_components = remove_extra_components(tree, current_prob);
      PROF(util::pstop(prof, "remove_extra_components()"));
      TRACE("extra: %s", tree.to_string(extra_components).c_str());

      PROF(util::pstart(prof, "remove_layers()"));
      remove_layers(tree, current_prob);
      PROF(util::pstop(prof, "remove_layers()"));

      PROF(util::pstart(prof, "complete_alpha_lists()"));
      std::vector<int> leaves = tree.get_leaves(current_prob);
      complete_alpha_lists(tree, alpha_list, vset, current_prob, leaves);
      PROF(util::pstop(prof, "complete_alpha_lists()"));

      PROF(util::pstart(prof, "refine()"));
      refine(tree, alpha_list, current_prob, leaves, prof);
      PROF(util::pstop(prof, "refine()"));

      PROF(util::pstart(prof, "promote()"));
      promote(tree, current_prob);
      PROF(util::pstop(prof, "promote()"));

      PROF(util::pstart(prof, "assemble()"));
      assemble(tree, alpha_list, current_prob, fp_neighbors, vset, prof);
      PROF(util::pstop(prof, "assemble()"));

      // clear all but visited
      PROF(util::pstart(prof, "clear all but visited"));
      for (auto c: tree.dfs_reverse_preorder_nodes(tree[current_prob].first_child)) {
        if (tree[c].is_leaf()) alpha_list[c].clear();
        tree[c].data.clear();
      }
      PROF(util::pstop(prof, "clear all but visited"));

      PROF(util::pstart(prof, "merge_components()"));
      merge_components(tree, current_prob, extra_components);
      PROF(util::pstop(prof, "merge_components()"));
    }

    result = tree[current_prob].first_child;
    current_prob = tree[current_prob].is_last_child() ? tree[current_prob].parent : tree[current_prob].right;
    TRACE("loop: %s", tree.to_string(result).c_str());
  }

  // set new root of the tree
  auto result_parent = tree[result].parent;
  tree.detach(result);
  tree.remove(result_parent);

  PROF(util::pstop(prof, "compute()"));
  TRACE("return: %s", tree.to_string(result).c_str());
  return result;
}
}  // namespace impl
}  // namespace compute
}  // namespace modular
