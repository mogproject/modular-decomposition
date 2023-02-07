#pragma once

#include "compute/MDSolver.hpp"

namespace modular {
typedef compute::Operation Operation;
typedef compute::VertexID VertexID;

class MDNode {
 public:
  Operation op;
  VertexID vertex;
  int vertices_begin;  // starting index of the vertices (leaves)
  int vertices_end;    // ending index + 1 of the vertices (leaves)

  MDNode(VertexID vertex = -1, Operation op = Operation::PRIME, VertexID vertices_begin = -1, VertexID vertices_end = -1)
      : op(op), vertex(vertex), vertices_begin(vertices_begin), vertices_end(vertices_end) {}
  MDNode(MDNode const &node)
      : op(node.op), vertex(node.vertex), vertices_begin(node.vertices_begin), vertices_end(node.vertices_end) {}
  void operator=(MDNode const &node) {
    vertex = node.vertex;
    op = node.op;
    vertices_begin = node.vertices_begin;
    vertices_end = node.vertices_end;
  }

  int size() const { return vertices_end - vertices_begin; }
  bool is_vertex_node() const { return vertex >= 0; }
  bool is_operation_node() const { return !is_vertex_node(); }
  bool is_prime_node() const { return is_operation_node() && op == Operation::PRIME; }
  bool is_join_node() const { return is_operation_node() && op == Operation::SERIES; }
  bool is_union_node() const { return is_operation_node() && op == Operation::PARALLEL; }

  std::string to_string() const {
    if (is_vertex_node()) return std::to_string(vertex);
    return is_prime_node() ? "P" : is_join_node() ? "J" : "U";
  }
};

std::ostream &operator<<(std::ostream &os, MDNode const &node);

class MDTree {
 private:
  ds::tree::IntRootedForest<MDNode> tree_;
  int root_;
  std::vector<VertexID> vertices_;

 public:
  MDTree() : root_(-1){};
  MDTree(ds::graph::Graph const &graph, bool sorted = false) {
    auto result = compute::MDSolver::compute(graph);
    if (result.second >= 0) {
      *this = MDTree(result.first, result.second);
      if (sorted) this->sort();
    }
  }

  MDTree(compute::CompTree const &comp_tree, int comp_root) {
    // get all leaves in DFS order (right-to-left)
    vertices_ = comp_tree.get_leaves(comp_root);
    std::reverse(vertices_.begin(), vertices_.end());  // convert to left-to-right

    int n = vertices_.size();

    // create vertex nodes
    for (int i = 0; i < n; ++i) tree_.create_node(MDNode(vertices_[i], Operation::PRIME, i, i + 1));

    // index mapping of internal nodes
    std::map<int, int> mapping;
    for (int i = 0; i < n; ++i) mapping[vertices_[i]] = i;

    // create internal nodes from the bottom
    auto bfs_order = comp_tree.bfs_nodes(comp_root);

    for (auto it = bfs_order.rbegin(); it != bfs_order.rend(); ++it) {
      if (comp_tree[*it].data.is_vertex_node()) continue;
      if (comp_tree[*it].data.is_problem_node()) throw std::invalid_argument("should not be a problem node");

      auto children = comp_tree.get_children(*it);
      auto op = comp_tree[*it].data.op_type;
      int idx_begin = n, idx_end = 0;
      for (auto c : children) {
        idx_begin = std::min(idx_begin, tree_[mapping[c]].data.vertices_begin);
        idx_end = std::max(idx_end, tree_[mapping[c]].data.vertices_end);
      }

      auto node_idx = tree_.create_node(MDNode(-1, op, idx_begin, idx_end));
      // printf("*it=%d, idx=%d, b=%d, e=%d\n", *it, node_idx, idx_begin, idx_end);
      for (auto cit = children.rbegin(); cit != children.rend(); ++cit) tree_.move_to(mapping[*cit], node_idx);
      mapping[*it] = node_idx;
      // std::cout << tree_.to_string(node_idx) << std::endl;
    }

    // set root
    root_ = mapping[comp_root];
  }

  int modular_width() const {
    if (root_ < 0) return 0;

    int ret = 0;
    for (auto c : tree_.dfs_reverse_preorder_nodes(root_)) {
      if (tree_[c].data.is_prime_node()) ret = std::max(ret, tree_[c].number_of_children());
    }
    return ret;
  }

  /**
   * @brief Sorts all nodes in lexicographical order.
   */
  void sort() {
    auto level_order = tree_.bfs_nodes(root_);

    // first pass (bottom-up): find the (lexicographically) smallest vertex for each module
    std::map<int, VertexID> min_label;
    for (auto it = level_order.rbegin(); it != level_order.rend(); ++it) {
      if (tree_[*it].is_leaf()) min_label[*it] = tree_[*it].data.vertex;
      if (!tree_[*it].is_root()) {
        auto p = tree_[*it].parent;
        auto nd = min_label[*it];
        min_label[p] = util::contains(min_label, p) ? std::min(min_label[p], nd) : nd;
      }
    }

    // second pass (top-down): reorder nodes and vertices
    std::map<int, int> new_begin;
    new_begin[root_] = 0;
    for (auto x : level_order) {
      if (tree_[x].is_leaf()) {
        vertices_[new_begin[x]] = tree_[x].data.vertex;  // stores the node id
        continue;
      }

      std::vector<std::pair<int, int>> cs;
      for (auto c : tree_.get_children(x)) cs.push_back({min_label[c], c});
      std::sort(cs.begin(), cs.end());

      auto idx = new_begin[x] + tree_[x].data.size();
      for (int i = static_cast<int>(cs.size()) - 1; i >= 0; --i) {
        auto c = cs[i].second;
        idx -= tree_[c].data.size();
        new_begin[c] = idx;
        tree_[c].data.vertices_end = idx + tree_[c].data.size();
        tree_[c].data.vertices_begin = idx;
        tree_.make_first_child(c);
      }
    }
  }

  ds::tree::IntRootedForest<MDNode> const &get_tree() const { return tree_; }
  int get_root() const { return root_; }
  VertexID get_vertex(int index) const {
    if (index < 0 || static_cast<int>(vertices_.size()) <= index) {
      throw std::invalid_argument(util::format("index (%d) out of range", index));
    }
    return vertices_[index];
  }
  std::string to_string() const { return tree_.to_string(root_); }
};

MDTree modular_decomposition(ds::graph::Graph const &graph, bool sorted = false);

std::pair<MDTree, double> modular_decomposition_time(ds::graph::Graph const &graph, bool sorted = false);
}  // namespace modular
