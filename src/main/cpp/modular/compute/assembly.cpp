#include "MDSolver.hpp"

namespace modular {
namespace compute {
namespace impl {

//================================================================================
//    Determine flags
//================================================================================
/**
 * @brief Obtains the flags for left cocomponent fragments.
 *
 * @note Read: MDNode::comp_number for each root
 */
static std::vector<bool> determine_left_cocomp_fragments(CompTree const &tree, VI const &ps, int pivot_index) {
  std::vector<bool> ret(ps.size());
  for (int i = 1; i < pivot_index; ++i) {
    ret[i] = tree[ps[i]].data.comp_number >= 0 && tree[ps[i - 1]].data.comp_number == tree[ps[i]].data.comp_number;
  }
  return ret;
}

/**
 * @brief Obtains the flags for right component fragments.
 *
 * @note Read: MDNode::comp_number for each root
 */
static std::vector<bool> determine_right_comp_fragments(CompTree const &tree, VI const &ps, int pivot_index) {
  std::vector<bool> ret(ps.size());
  for (int i = pivot_index + 1; i < static_cast<int>(ps.size()) - 1; ++i) {
    ret[i] = tree[ps[i]].data.comp_number >= 0 && tree[ps[i]].data.comp_number == tree[ps[i + 1]].data.comp_number;
  }
  return ret;
}

/**
 * @brief Obtains the flags for the right layer neighbor.
 *
 * @note Read: MDNode::tree_number for each root and leaf
 *             MDLeaf::alpha       for each leaf
 */
static std::vector<bool> determine_right_layer_neighbor(CompTree const &tree, VI const *alpha_list, VI const &ps, int pivot_index) {
  std::vector<bool> ret(ps.size());
  for (int i = pivot_index + 1; i < static_cast<int>(ps.size()); ++i) {
    int current_tree = ps[i];
    int current_tree_num = tree[current_tree].data.tree_number;

    for (auto leaf : tree.get_leaves(current_tree)) {
      for (auto a : alpha_list[leaf]) {
        if (tree[a].data.tree_number > current_tree_num) {
          ret[i] = true;
          goto outer_loop_exit;
        }
      }
    }
  outer_loop_exit:;
  }
  return ret;
}

//================================================================================
//    Compute factorized-permutation edges
//================================================================================
/**
 * @brief Computes the edges between factorized-permutation elements.
 *
 * @note Read  : MDLeaf::alpha       for each leaf
 *       Update: MDLeaf::comp_number for each leaf
 * @return vector of neighboring factorized-permutation indices
 */
static VVV compute_fact_perm_edges(CompTree &tree, VI const *alpha_list, VI const &ps) {
  // TRACE("start: %s\n", to_string().c_str());
  int k = static_cast<int>(ps.size());
  VVV neighbors(k);
  std::vector<int> element_sizes(k);

  // initialize
  for (int i = 0; i < k; ++i) {
    for (auto leaf : tree.get_leaves(ps[i])) {
      tree[leaf].data.comp_number = i;  // reset the comp number to index
      ++element_sizes[i];               // increment the element size
    }
  }

  // find joins
  for (int i = 0; i < k; ++i) {
    std::vector<int> candidates;
    std::vector<int> marks(k);

    // enumerate all edges
    for (auto leaf : tree.get_leaves(ps[i])) {
      for (auto a : alpha_list[leaf]) {
        int j = tree[a].data.comp_number;
        candidates.push_back(j);
        ++marks[j];
      }
    }

    for (auto j : candidates) {
      if (element_sizes[i] * element_sizes[j] == marks[j]) {
        // found a join between i and j
        neighbors[i].push_back(j);
        marks[j] = 0;  // reset marks so that there will be no duplicates
      }
    }
  }
  // TRACE("finish: %s\n", to_string().c_str());
  return neighbors;
}

//================================================================================
//    Compute mu-values
//================================================================================
/**
 * @brief Computes the mu-value for each factorizing permutation element.
 */
static std::vector<int> compute_mu(CompTree const &tree, VI const &ps, int pivot_index, VVV const &neighbors) {
  // TRACE("start: %s\n", to_string().c_str());
  std::vector<int> mu(ps.size());

  // initialize mu-values
  for (int i = 0; i < static_cast<int>(ps.size()); ++i) mu[i] = i < pivot_index ? pivot_index : 0;

  // mu-values determined only by looking at elements to the left of pivot.
  for (int i = 0; i < pivot_index; ++i) {
    for (auto j : neighbors[i]) {
      // Neighbor to left of pivot is universal to all up to current,
      // and also adjacent to current, so mu gets updated to next.
      if (mu[j] == i) mu[j] = i + 1;

      // Current has an edge past previous farthest edge, so must update mu.
      if (j > mu[i]) mu[i] = j;
    }
  }

  // TRACE("finish: %s\n", to_string().c_str());
  return mu;
}

//================================================================================
//    Delineate
//================================================================================
struct DelineateState {
  int lb, rb, left_last_in, right_last_in;
};

static bool compose_series(std::vector<bool> const &lcocomp, std::vector<int> const &mu, DelineateState &st) {
  bool ret = false;
  while (0 <= st.lb && mu[st.lb] <= st.right_last_in && !lcocomp[st.lb]) {
    ret = true;
    st.left_last_in = st.lb;
    --st.lb;
  }
  return ret;
}

static bool compose_parallel(std::vector<bool> const &rcomp, std::vector<bool> const &rlayer,
                             std::vector<int> const &mu, DelineateState &st) {
  bool ret = false;
  while (st.rb < static_cast<int>(rcomp.size()) && st.left_last_in <= mu[st.rb] && !rcomp[st.rb] && !rlayer[st.rb]) {
    ret = true;
    st.right_last_in = st.rb;
    ++st.rb;
  }
  return ret;
}

static bool compose_prime(std::vector<bool> const &lcocomp, std::vector<bool> const &rcomp,
                          std::vector<bool> const &rlayer, std::vector<int> const &mu, DelineateState &st) {
  std::queue<int> left_q, right_q;

  do {
    left_q.push(st.lb);
    st.left_last_in = st.lb--;
  } while (lcocomp[st.left_last_in]);

  while (!left_q.empty() || !right_q.empty()) {
    // add elements from the left of the pivot
    while (!left_q.empty()) {
      auto current_left = left_q.front();
      left_q.pop();

      // add all elements up to mu
      while (st.right_last_in < mu[current_left]) {
        do {
          right_q.push(st.rb);
          st.right_last_in = st.rb++;
          if (rlayer[st.right_last_in]) return true;  // exit here
        } while (rcomp[st.right_last_in]);
      }
    }

    // add elements to the right of the pivot
    while (!right_q.empty()) {
      auto current_right = right_q.front();
      right_q.pop();

      while (mu[current_right] < st.left_last_in) {
        do {
          left_q.push(st.lb);
          st.left_last_in = st.lb--;
        } while (lcocomp[st.left_last_in]);
      }
    }
  }
  return false;
}

/**
 * @brief Finds boundaries for each module.
 *
 * @param lcocomp left-cocomp flags
 * @param rcomp  right-comp flags
 * @param rlayer right-layer flags
 * @param mu  mu values
 * @return boundaries
 */
static VII delineate(int pivot_index, std::vector<bool> const &lcocomp, std::vector<bool> const &rcomp,
                     std::vector<bool> const &rlayer, std::vector<int> const &mu) {
  // TRACE("start: %s\n", to_string().c_str());
  VII ret;

  // current boundary
  DelineateState st = {pivot_index - 1, pivot_index + 1, pivot_index, pivot_index};
  int k = static_cast<int>(lcocomp.size());

  while (0 <= st.lb && st.rb < k) {
    if (!compose_series(lcocomp, mu, st)) {
      if (!compose_parallel(rcomp, rlayer, mu, st)) {
        if (compose_prime(lcocomp, rcomp, rlayer, mu, st)) {
          // added to the module an element to the right of x with an edge to a layer to its right,
          // so the module must be the entire graph in this case
          st.left_last_in = 0;
          st.right_last_in = k - 1;
          st.lb = st.left_last_in - 1;
          st.rb = st.right_last_in + 1;
        }
      }
    }

    // delineate the module just found
    ret.push_back({st.left_last_in, st.right_last_in});
  }
  return ret;
}

//================================================================================
//    Assemble tree
//================================================================================
static int assemble_tree(CompTree &tree, VI const &ps, int pivot_index, VII const &boundaries) {
  // TRC << "start:" << to_string() << ",boundaries=" << boundaries << std::endl;
  int k = static_cast<int>(ps.size());
  auto lb = pivot_index - 1;
  auto rb = pivot_index + 1;
  auto last_module = ps[pivot_index];

  int sz = static_cast<int>(boundaries.size());
  int i = 0;  // boundary index

  while (0 <= lb || rb < k) {
    auto lbound = i < sz ? boundaries[i].first : 0;
    auto rbound = i < sz ? boundaries[i].second : static_cast<int>(ps.size()) - 1;
    ++i;

    // create the spine
    auto new_module = tree.create_node(MDComputeNode::new_operation_node(Operation::PRIME));
    tree.move_to(last_module, new_module);

    bool added_nbrs = false;
    bool added_nonnbrs = false;

    // add the subtrees of the new module from N(x)
    for (; lb >= lbound; --lb) {
      added_nbrs = true;
      tree.move_to(ps[lb], new_module);
    }

    // add the subtrees of the new module from \overline{N(x)}
    for (; rb <= rbound; ++rb) {
      added_nonnbrs = true;
      tree.move_to(ps[rb], new_module);
    }

    tree[new_module].data.op_type = added_nbrs && added_nonnbrs ? Operation::PRIME
                                    : added_nbrs                ? Operation::SERIES
                                                                : Operation::PARALLEL;
    last_module = new_module;
  }

  return last_module;
}

//================================================================================
//    Cleaning
//================================================================================
static void remove_degenerate_duplicates(CompTree &tree, int index) {
  auto nodes = tree.bfs_nodes(index);

  for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
    if (*it == index) break;

    auto c = tree[*it];
    auto p = tree[c.parent];
    if (c.data.op_type == p.data.op_type && c.data.op_type != Operation::PRIME) {
      tree.replace_by_children(*it);
      tree.remove(*it);
    }
  }
}

//================================================================================
//    Main process
//================================================================================
void assemble(CompTree &tree, VI const *alpha_list, int prob) {
  if (tree[prob].is_leaf()) throw std::invalid_argument("roots must not be empty");

  // build permutation
  std::vector<int> ps;  // target problems
  int current_pivot = tree[prob].data.vertex;
  int pivot_index = -1;

  for (auto p : tree.get_children(prob)) {
    if (p == current_pivot) pivot_index = ps.size();
    ps.push_back(p);
  }
  if (pivot_index < 0) throw std::invalid_argument("roots must include a pivot");

  // main logic
  auto lcocomp = determine_left_cocomp_fragments(tree, ps, pivot_index);
  auto rcomp = determine_right_comp_fragments(tree, ps, pivot_index);
  auto rlayer = determine_right_layer_neighbor(tree, alpha_list, ps, pivot_index);
  auto neighbors = compute_fact_perm_edges(tree, alpha_list, ps);
  auto mu = compute_mu(tree, ps, pivot_index, neighbors);
  auto boundaries = delineate(pivot_index, lcocomp, rcomp, rlayer, mu);
  auto root = assemble_tree(tree, ps, pivot_index, boundaries);
  remove_degenerate_duplicates(tree, root);

  // replace the problem node with the result
  tree.replace_children(prob, root);
}

}  // namespace impl
}  // namespace compute
}  // namespace modular
