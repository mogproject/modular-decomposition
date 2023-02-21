#include "MDSolver.hpp"

namespace modular {
namespace compute {

namespace impl {

int remove_extra_components(CompTree &tree, int prob) {
  TRACE("start: %s", tree.to_string(prob).c_str());

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

  TRACE("return: %s", tree.to_string(ret).c_str());
  return ret;
}

/**
 * @brief Replaces the subproblems of this problem with their MD trees.
 */
void remove_layers(CompTree &tree, int prob) {
  TRACE("start: %s", tree.to_string(prob).c_str());

  for (auto c = tree[prob].first_child; tree.is_valid(c);) {
    auto nxt = tree[c].right;
    tree.replace(c, tree[c].first_child);  // should have only one child
    tree.remove(c);
    c = nxt;
  }

  TRACE("finish: %s", tree.to_string(prob).c_str());
}

/**
 * @brief Makes alpha lists in this subproblem symmetric and irredundant.
 */
void complete_alpha_lists(CompTree &tree, VVV &alpha_list, ds::FastSet &vset, int prob, std::vector<int> &leaves) {
  TRACE("start: %s", tree.to_string(prob).c_str());

  // complete the list
  for (auto v : leaves) {
    assert(v >= 0);
    for (auto a : alpha_list[v]) {
      assert(a >= 0);
      alpha_list[a].push_back(v);
    }
  }

  // remove duplicate entries (in-place)
  for (auto v : leaves) {
    auto &vs = alpha_list[v];
    vset.clear();

    std::size_t len = vs.size();
    for (std::size_t i = 0; i < len;) {
      auto a = vs[i];
      if (vset.get(a)) {
        // found a duplicate; swap with the last element
        vs[i] = vs[--len];
        vs.pop_back();
      } else {
        vset.set(a);
        ++i;
      }
    }
  }
}

void merge_components(CompTree &tree, int prob, int new_components) {
  TRACE("start: prob=%s, new=%s", tree.to_string(prob).c_str(), tree.to_string(new_components).c_str());

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

  TRACE("finish: %s", tree.to_string(prob).c_str());
}

}  // namespace impl
}  // namespace compute
}  // namespace modular
