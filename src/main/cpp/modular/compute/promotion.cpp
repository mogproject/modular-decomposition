#include "MDSolver.hpp"

namespace modular {
namespace compute {

namespace impl {

static void promote_one_node(CompTree &tree, int index, SplitDirection split_type) {
  // non-recursive implementation
  if (tree[index].is_leaf()) return;

  std::stack<std::pair<bool, int>> st;
  st.push({false, index});
  st.push({true, tree[index].first_child});

  while (!st.empty()) {
    auto x = st.top();
    auto nd = x.second;
    st.pop();

    if (x.first) {
      // forward pass
      auto right = tree[nd].right;
      if (tree.is_valid(right)) {
        st.push({true, right});  // point to the next node
      }
      if (tree[nd].data.is_split_marked(split_type)) {
        auto p = tree[nd].parent;
        assert(tree.is_valid(p));

        if (split_type == SplitDirection::LEFT) {
          tree.move_to_before(nd, p);
        } else {
          tree.move_to_after(nd, p);
        }
        if (tree[nd].has_child()) {
          st.push({false, nd});
          st.push({true, tree[nd].first_child});  // dig into the children
        }
      }
    } else {
      // backward pass
      if (tree[nd].is_leaf() && tree[nd].data.is_operation_node()) {
        tree.remove(nd);
      } else if (tree[nd].has_only_one_child()) {
        tree.replace_by_children(nd);
        tree.remove(nd);
      }
    }
  }
}

static void promote_one_direction(CompTree &tree, int index, SplitDirection split_type) {
  for (auto c : tree.get_children(index)) promote_one_node(tree, c, split_type);
}

void promote(CompTree &tree, int prob) {
  TRACE("start: %s", tree.to_string(prob).c_str());

  promote_one_direction(tree, prob, SplitDirection::LEFT);
  promote_one_direction(tree, prob, SplitDirection::RIGHT);

  TRACE("finish: %s", tree.to_string(prob).c_str());
}

}  // namespace impl
}  // namespace compute
}  // namespace modular
