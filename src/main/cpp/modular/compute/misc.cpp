#include "MDSolver.hpp"

namespace modular {
namespace compute {

namespace impl {

int remove_extra_components(CompTree &tree, int prob) {
  TRACE("start: %s\n", tree.to_string(prob).c_str());

  auto subprob = tree[prob].first_child;
  while (tree.is_valid(subprob) && tree[subprob].data.connected) subprob = tree[subprob].right;

  int ret = -1;
  if (tree.is_valid(subprob)) {
    ret = tree[subprob].first_child;
    assert(ret >= 0);
    tree.detach(ret);
    assert(tree[subprob].is_leaf());
    tree.remove(subprob);
  }

  TRACE("return: %s\n", tree.to_string(ret).c_str());
  return ret;
}

/**
 * @brief Replaces the subproblems of this problem with their MD trees.
 */
void remove_layers(CompTree &tree, int prob) {
  TRACE("start: %s\n", tree.to_string(prob).c_str());

  for (auto c : tree.get_children(prob)) {
    tree.replace_by_children(c);
    tree.remove(c);
  }

  TRACE("finish: %s\n", tree.to_string(prob).c_str());
}

/**
 * @brief Makes alpha lists in this subproblem symmetric and irredundant.
 */
void complete_alpha_lists(CompTree &tree, VI alpha_list[], ds::FastSet &vset, int prob) {
  TRACE("start: %s\n", tree.to_string(prob).c_str());

  // complete the list
  for (auto v : tree.get_leaves(prob)) {
    assert(v >= 0);
    for (int a : alpha_list[v]) {
      assert(a >= 0);
      alpha_list[a].push_back(v);
    }
  }
  // remove duplicate entries
  for (auto v : tree.get_leaves(prob)) {
    vset.clear();
    std::vector<int> result;
    for (auto a : alpha_list[v]) {
      if (!vset.get(a)) {
        result.push_back(a);
        vset.set(a);
      }
    }
    alpha_list[v] = result;
  }
}

void merge_components(CompTree &tree, int prob, int new_components) {
  TRACE("start: prob=%s, new=%s\n", tree.to_string(prob).c_str(), tree.to_string(new_components).c_str());

  if (!tree.is_valid(new_components)) return;

  auto fc = tree[prob].first_child;

  if (tree[new_components].data.op_type == Operation::PARALLEL) {
    if (tree[fc].data.op_type == Operation::PARALLEL) {
      tree.add_children_from(new_components, fc);
    } else {
      tree.move_to(fc, new_components);
    }
    tree.move_to(new_components, prob);
  } else {
    auto new_root = tree.create_node(MDComputeNode::new_operation_node(Operation::PARALLEL));
    tree.move_to(new_root, prob);
    tree.move_to(new_components, new_root);
    tree.move_to(fc, new_root);
  }

  TRACE("finish: %s\n", tree.to_string(problem).c_str());
}

}  // namespace impl
}  // namespace compute
}  // namespace modular
