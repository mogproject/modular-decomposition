#include "MDSolver.hpp"

namespace modular {
namespace compute {

namespace impl {

static bool is_pivot_layer(CompTree& tree, int index) {
  auto& node = tree[index];

  if (!tree.is_valid(node.parent)) return false;
  auto& p = tree[node.parent];

  return p.data.is_problem_node() && p.data.vertex == node.first_child;
}

static void pull_forward(CompTree& tree, VertexID v) {
  TRACE("pull_forward(): v=%d", v);

  auto current_layer = tree[v].parent;
  assert(tree.is_valid(current_layer));

  if (tree[current_layer].data.connected) return;
  assert(tree[current_layer].data.is_problem_node());

  auto prev_layer = tree[current_layer].left;
  assert(tree.is_valid(prev_layer));

  // form a new layer
  TRACE("tree[prev_layer].data.active=%d, is_pivot_layer()=%d", tree[prev_layer].data.active, is_pivot_layer(tree, prev_layer));

  if (tree[prev_layer].data.active || is_pivot_layer(tree, prev_layer)) {
    auto new_layer = tree.create_node(MDComputeNode::new_problem_node(true));  // connected problem
    tree.move_to_before(new_layer, current_layer);
    prev_layer = new_layer;
    TRACE("new layer formed: %s", tree.to_string(tree[prev_layer].parent).c_str());
  }

  if (tree[prev_layer].data.connected) tree.move_to(v, prev_layer);
  if (tree[current_layer].is_leaf()) tree.remove(current_layer);  // all leaves in this layer have been removed
}

void process_neighbors(             //
    ds::graph::Graph const& graph,  //
    CompTree& tree,                 //
    VVV& alpha_list,                //
    bool const visited[],           //
    VertexID pivot,                 //
    int current_prob,               //
    int nbr_prob                    //
) {
  TRACE("enter: pivot=%d, current_prob=%d, nbr_prob=%d", pivot, current_prob, nbr_prob);

  for (auto nbr : graph.neighbors(pivot)) {
    // TRACE("nbr=%d\n", nbr);
    if (visited[nbr]) {
      alpha_list[nbr].push_back(pivot);  // add pivot to nbr's alpha list
    } else if (tree[nbr].parent == current_prob) {
      // nbr_prob must be a valid node
      tree.move_to(nbr, nbr_prob);
    } else {
      TRACE("pull_forward(): nbr=%d", nbr);
      pull_forward(tree, nbr);
    }
  }
}

/**
 * @brief Selects a pivot vertex from this subproblem.
 *
 * @return MDComputeNode parent of the subproblems
 */
int do_pivot(ds::graph::Graph const& graph,  //
             CompTree& tree,                 //
             VVV& alpha_list,                //
             bool const visited[],           //
             int prob,                       //
             VertexID pivot                  //
) {
  TRACE("start: %s", tree.to_string(prob).c_str());
  TRACE("pivot: %d", pivot);

  // Replace this subproblem with a new one sharing the same attributes.
  // Reuse the current recursive subproblem for non-neighbors of p.
  // Order must be neighbors, pivot, non-neighbors from the left.
  auto replacement = tree.create_node(tree[prob].data);
  tree.swap(prob, replacement);
  tree.move_to(prob, replacement);
  tree[replacement].data.vertex = pivot;  // set pivot

  // clear attributes
  tree[prob].data.active = false;
  tree[prob].data.connected = false;
  tree[prob].data.vertex = -1;

  // Create a subproblem for the pivot.
  auto pivot_prob = tree.create_node(MDComputeNode::new_problem_node(true));  // connected problem
  tree.move_to(pivot_prob, replacement);
  tree.move_to(pivot, pivot_prob);

  // Create a subproblem for the neighbors of p.
  auto nbr_prob = tree.create_node(MDComputeNode::new_problem_node(true));  // connected problem
  tree.move_to(nbr_prob, replacement);
  process_neighbors(graph, tree, alpha_list, visited, pivot, prob, nbr_prob);

  // Clean up: no non-neighbors of p in this problem.
  if (tree[prob].is_leaf()) tree.remove(prob);

  // Clean up: no neighbors of p in this problem.
  if (tree[nbr_prob].is_leaf()) tree.remove(nbr_prob);

  TRACE("return: %d: %s", replacement, tree.to_string(replacement).c_str());
  return replacement;
}

}  // namespace impl
}  // namespace compute
}  // namespace modular
