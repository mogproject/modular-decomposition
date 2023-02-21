#pragma once

#include <list>
#include <queue>
#include <stack>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "util/util.hpp"

//================================================================================
//    Macros
//================================================================================

// validation: on
// #define VALIDATE(s) s

// validation: off
#define VALIDATE(s)

#define FOR_EACH_CHILD(c, p) for (auto c = nodes_[(p)].first_child; (c) != NOT_AVAILABLE; (c) = nodes_[(c)].right)

namespace ds {
namespace tree {

/**
 * @brief Tree representation where each node has a unique integer.
 *
 * @tparam T type for node data
 */
template <typename T>
class IntRootedForest {
 public:
  static int const NOT_AVAILABLE = -1;

 private:
  /**
   * @brief Represents a node in the forest.
   */
  class Node {
   public:
    // Fields
    T data;
    int parent;
    int left;
    int right;
    int first_child;
    int num_children;
    bool alive;

   public:
    // Constructor
    Node(T const& data)
        : data(data),
          parent(NOT_AVAILABLE),
          left(NOT_AVAILABLE),
          right(NOT_AVAILABLE),
          first_child(NOT_AVAILABLE),
          num_children(0),
          alive(true) {}

    // Copy constructor
    Node(Node const& node)
        : data(node.data),
          parent(node.parent),
          left(node.left),
          right(node.right),
          first_child(node.first_child),
          num_children(node.num_children),
          alive(node.alive) {}

    // Assignment operator
    void operator=(Node const& node) {
      data = node.data;
      parent = node.parent;
      left = node.left;
      right = node.right;
      first_child = node.first_child;
      num_children = node.num_children;
      alive = node.alive;
    }

    // Utilities
    bool is_alive() const { return alive; }
    bool is_root() const { return parent == NOT_AVAILABLE; }
    bool has_parent() const { return parent != NOT_AVAILABLE; }
    bool is_first_child() const { return has_parent() && left == NOT_AVAILABLE; }
    bool is_last_child() const { return has_parent() && right == NOT_AVAILABLE; }
    bool is_leaf() const { return first_child == NOT_AVAILABLE; }
    bool has_child() const { return first_child != NOT_AVAILABLE; }
    bool has_only_one_child() const { return num_children == 1; }
    int number_of_children() const { return num_children; }
  };

  std::vector<Node> nodes_;
  std::queue<int> removed_;  // queue of removed indices
  std::size_t num_live_nodes_;

 public:
  IntRootedForest() : nodes_(), removed_(), num_live_nodes_(0) {}

  /**
   * @brief Copy constructor
   */
  IntRootedForest(IntRootedForest const& tree) {
    num_live_nodes_ = tree.num_live_nodes_;
    for (int i = 0; i < static_cast<int>(tree.capacity()); ++i) {
      nodes_.push_back(tree.nodes_[i]);
      if (!tree.nodes_[i].alive) removed_.push(i);
    }
  }

  //================================================================================
  //    Node Access
  //================================================================================
  // array subscript operator for writing
  Node& operator[](std::size_t index) {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("invalid index"));
    return nodes_[index];
  }

  // array subscript operator for reading
  Node const& operator[](std::size_t index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("invalid index"));
    return nodes_[index];
  }

  // get nodes in the one-level lower
  std::vector<int> get_children(int index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("get_children: invalid index"));

    std::vector<int> ret;
    FOR_EACH_CHILD(c, index) ret.push_back(c);
    return ret;
  }

  //================================================================================
  //    Node Traversal
  //================================================================================
  /**
   * @brief Returns a list of pairs of nodes and directions of the traversal
   *        in a depth-first-search (left to right) pre-ordering starting at index.
   *
   * @param index start node
   * @return std::vector<std::pair<int, int>> list of [node, entering (true) or leaving (false)]
   *         entering: traverse from parent to node
   *         leaving:  traverse from node to parent
   */
  std::vector<std::pair<int, int>> dfs_preorder_edges(int index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("dfs_preorder_edges: invalid index"));

    std::vector<std::pair<int, int>> ret, stack;
    stack.push_back({index, false});
    stack.push_back({index, true});

    while (!stack.empty()) {
      auto x = stack.back();
      ret.push_back(x);
      if (x.second) {
        for (auto it = get_children((x.first)).rbegin(); it; it = it.next()) {
          stack.push_back({*it, false});
          stack.push_back({*it, true});
        }
      }
    }

    return ret;
  }

  std::vector<std::pair<int, int>> dfs_reverse_preorder_edges(int index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("dfs_preorder_edges: invalid index"));

    std::vector<std::pair<int, int>> ret, stack;
    stack.push_back({index, false});
    stack.push_back({index, true});

    while (!stack.empty()) {
      auto x = stack.back();
      ret.push_back(x);
      if (x.second) {
        FOR_EACH_CHILD(c, x.first) {
          stack.push_back({c, false});
          stack.push_back({c, true});
        }
      }
    }

    return ret;
  }

  std::vector<int> bfs_nodes(int index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("bfs_nodes: invalid index"));

    std::vector<int> ret;
    std::queue<int> q;

    q.push(index);
    while (!q.empty()) {
      auto x = q.front();
      q.pop();
      ret.push_back(x);
      FOR_EACH_CHILD(c, x) q.push(c);
    }
    return ret;
  }

  std::vector<int> dfs_preorder_nodes(int index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("dfs_preorder_nodes: invalid index"));

    std::vector<int> ret, stack;
    stack.push_back(index);

    while (!stack.empty()) {
      auto x = stack.back();
      stack.pop_back();
      ret.push_back(x);

      auto cs = get_children((x));
      for (auto it = cs.rbegin(); it != cs.rend(); ++it) stack.push_back(*it);
    }
    return ret;
  }

  std::vector<int> dfs_reverse_preorder_nodes(int index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("dfs_reverse_preorder_nodes: invalid index"));

    std::vector<int> ret, stack;
    stack.push_back(index);

    while (!stack.empty()) {
      auto x = stack.back();
      stack.pop_back();
      ret.push_back(x);

      FOR_EACH_CHILD(c, x) stack.push_back(c);
    }
    return ret;
  }

  /**
   * @brief Returns a list of the leaves of this subtree, from the left to the right.
   *
   * @param index
   * @return std::vector<int>
   */
  std::vector<int> get_leaves(int index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("get_leaves: invalid index"));

    std::vector<int> ret;
    for (auto x : dfs_reverse_preorder_nodes(index)) {
      if (nodes_[x].is_leaf()) ret.push_back(x);
    }
    return ret;
  }

  std::vector<int> get_ancestors(int index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("get_ancestors: invalid index"));

    std::vector<int> ret;
    for (auto p = nodes_[index].parent; p != NOT_AVAILABLE; p = nodes_[p].parent) ret.push_back(p);
    return ret;
  }

  int get_root(int index) const {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("get_root: invalid index"));

    int ret = NOT_AVAILABLE;
    for (auto p = index; p != NOT_AVAILABLE; p = nodes_[p].parent) ret = p;
    return ret;
  }

  //================================================================================
  //    Tree Properties
  //================================================================================
  std::size_t size() const { return num_live_nodes_; }
  std::size_t capacity() const { return nodes_.size(); }
  bool is_valid(int index) const {
    return 0 <= index && index < static_cast<int>(capacity()) && nodes_[index].is_alive();
  }

  //================================================================================
  //    Node Addition and Removal
  //================================================================================
  int create_node() { return create_node_impl(T()); }

  template <typename A>
  int create_node(A const& arg) {
    return create_node_impl(T(arg));
  }

 private:
  std::size_t create_node_impl(T const& x) {
    int index = NOT_AVAILABLE;

    if (removed_.empty()) {
      // add new slot
      index = nodes_.size();
      nodes_.push_back(x);
    } else {
      // reuse removed slot
      index = removed_.front();
      removed_.pop();
      if (nodes_[index].is_alive()) throw std::runtime_error("create_node_impl: reusing live node");
      nodes_[index] = Node(x);
    }
    ++num_live_nodes_;
    return index;
  }

 public:
  std::vector<int> get_roots() const {
    std::vector<int> ret;
    for (int i = 0; i < static_cast<int>(nodes_.size()); ++i) {
      if (nodes_[i].alive && nodes_[i].is_root()) ret.push_back(i);
    }
    return ret;
  }

  void remove(int index) {
    VALIDATE(if (!is_valid(index)) throw std::invalid_argument("remove: invalid index"));
    detach(index);
    VALIDATE(if (!nodes_[index].is_leaf()) throw std::invalid_argument("remove: must be a leaf"));

    --num_live_nodes_;
    nodes_[index].alive = false;
    removed_.push(index);
  }

  //================================================================================
  //    Modification
  //================================================================================
 private:
  void add_child(int parent, int child) {
    VALIDATE({
      if (!is_valid(parent)) throw std::invalid_argument("add_child: parent invalid index");
      if (!is_valid(child)) throw std::invalid_argument("add_child: child invalid index");
    });
    auto& p = nodes_[parent];
    auto& c = nodes_[child];

    VALIDATE({
      if (!c.is_root()) throw std::invalid_argument("add_child: child must be a root");
    });

    if (p.has_child()) {
      nodes_[p.first_child].left = child;
      nodes_[child].right = p.first_child;
    }

    p.first_child = child;
    c.parent = parent;
    ++p.num_children;
  }

 public:
  void detach(int index) {
    VALIDATE({
      if (!is_valid(index)) throw std::invalid_argument("detach: invalid index");
    });

    auto& node = nodes_[index];
    if (node.parent != NOT_AVAILABLE) nodes_[node.parent].num_children--;
    if (node.is_first_child()) nodes_[node.parent].first_child = node.right;
    if (node.left != NOT_AVAILABLE) nodes_[node.left].right = node.right;
    if (node.right != NOT_AVAILABLE) nodes_[node.right].left = node.left;

    node.parent = NOT_AVAILABLE;
    node.left = NOT_AVAILABLE;
    node.right = NOT_AVAILABLE;
  };

  void swap(int a, int b) {
    VALIDATE({
      if (!is_valid(a)) throw std::invalid_argument("swap: a invalid index");
      if (!is_valid(b)) throw std::invalid_argument("swap: b invalid index");
      if (get_root(a) == get_root(b)) throw std::invalid_argument("swap: a and b must belong to different trees")
    });
    // if (a == b) return; // never happens

    auto& na = nodes_[a];
    auto& nb = nodes_[b];

    if (na.is_first_child()) nodes_[na.parent].first_child = b;
    if (na.left != NOT_AVAILABLE) nodes_[na.left].right = b;
    if (na.right != NOT_AVAILABLE) nodes_[na.right].left = b;

    if (nb.is_first_child()) nodes_[nb.parent].first_child = a;
    if (nb.left != NOT_AVAILABLE) nodes_[nb.left].right = a;
    if (nb.right != NOT_AVAILABLE) nodes_[nb.right].left = a;

    std::swap(nodes_[a].parent, nodes_[b].parent);
    std::swap(nodes_[a].left, nodes_[b].left);
    std::swap(nodes_[a].right, nodes_[b].right);
  }

  /**
   * @brief Replaces this node and its children with the given node.
   * @param index node to be replaced
   * @param replace_by not to replace
   */
  void replace(int index, int replace_by) {
    VALIDATE({
      if (!is_valid(index)) throw std::invalid_argument("replace: invalid index");
      if (!is_valid(replace_by)) throw std::invalid_argument("replace: replace_by invalid index");
      if (index == replace_by) throw std::invalid_argument("replace: replace_by must differ from index");
      if (util::contains(get_ancestors(index), replace_by)) {
        throw std::invalid_argument("replace: replace_by cannot be an ancestor of index");
      }
    });

    detach(replace_by);
    swap(index, replace_by);
  }

  void move_to(int index, int new_parent) {
    VALIDATE({
      if (!is_valid(index)) throw std::invalid_argument("move_to: invalid index");
      if (!is_valid(new_parent)) throw std::invalid_argument("move_to: new_parent invalid index");
      if (index == new_parent) throw std::invalid_argument("move_to: index and new_parent cannot be the same");
    });

    detach(index);
    add_child(new_parent, index);
  }

  /**
   * @brief Moves this node to the left sibling of the given node.
   * @param node node that is not a root
   */
  void move_to_before(int index, int target) {
    VALIDATE({
      if (!is_valid(index)) throw std::invalid_argument("move_to_after: invalid index");
      if (!is_valid(target)) throw std::invalid_argument("move_to_after: target invalid index");
      if (nodes_[target].is_root()) throw std::invalid_argument("move_to_before: target must not be a root");
      if (index == target) throw std::invalid_argument("move_to_after: index and target cannot be the same");
      if (util::contains(get_ancestors(target), index)) {
        throw std::invalid_argument("replace: index cannot be an ancestor of target");
      }
    });

    detach(index);

    auto& x = nodes_[index];
    auto& y = nodes_[target];

    x.parent = y.parent;
    x.left = y.left;
    x.right = target;

    nodes_[y.parent].num_children++;
    if (y.is_first_child()) nodes_[y.parent].first_child = index;
    if (!y.is_first_child()) nodes_[y.left].right = index;
    y.left = index;
  }

  /**
   * @brief Moves this node to the right sibling of the given node.
   * @param node node that is not a root
   */
  void move_to_after(int index, int target) {
    VALIDATE({
      if (!is_valid(index)) throw std::invalid_argument("move_to_after: invalid index");
      if (!is_valid(target)) throw std::invalid_argument("move_to_after: target invalid index");
      if (nodes_[target].is_root()) throw std::invalid_argument("move_to_after: target must not be a root");
      if (index == target) throw std::invalid_argument("move_to_after: index and target cannot be the same");
      if (util::contains(get_ancestors(target), index)) {
        throw std::invalid_argument("move_to_after: index cannot be an ancestor of target");
      }
    });

    detach(index);

    auto& x = nodes_[index];
    auto& y = nodes_[target];

    x.parent = y.parent;
    x.left = target;
    x.right = y.right;

    nodes_[y.parent].num_children++;
    if (y.right != NOT_AVAILABLE) nodes_[y.right].left = index;
    y.right = index;
  }

  /**
   * @brief Moves this node to the first among all its siblings.
   */
  void make_first_child(int index) {
    VALIDATE({
      if (!is_valid(index)) throw std::invalid_argument("make_first_child: invalid index");
    });

    if (nodes_[index].is_root() || nodes_[index].is_first_child()) return;  // do nothing

    move_to_before(index, nodes_[nodes_[index].parent].first_child);
  }

  /**
   * @brief Moves all the children of the given node to this node.
   * @param node node whose children are to be added to this node's children
   */
  void add_children_from(int index, int target) {
    VALIDATE({
      if (!is_valid(index)) throw std::invalid_argument("add_children_from: invalid index");
      if (!is_valid(target)) throw std::invalid_argument("add_children_from: target invalid index");
      if (util::contains(get_ancestors(index), target)) {
        throw std::invalid_argument("add_children_from: target cannot be an ancestor of index");
      }
    });

    if (index == target) return;  // do nothing

    auto& node = nodes_[index];
    auto& t = nodes_[target];

    FOR_EACH_CHILD(c, target) {
      nodes_[c].parent = index;
      if (nodes_[c].is_last_child()) {
        nodes_[c].right = node.first_child;
        if (node.has_child()) nodes_[node.first_child].left = c;
        break;  // necessary; otherwise it may not stop
      }
    }

    if (t.has_child()) node.first_child = t.first_child;
    t.first_child = NOT_AVAILABLE;
    node.num_children += t.num_children;
    t.num_children = 0;
  }

  /**
   * @brief Replaces the given node by its own children.
   * This original node will be detached form its tree but not removed.
   */
  void replace_by_children(int index) {
    VALIDATE({
      if (!is_valid(index)) throw std::invalid_argument("replace_by_children: invalid index");
      auto& node = nodes_[index];
      if (node.is_root()) throw std::invalid_argument("replace_by_children:this must not be a root");
    });

    for (auto c = nodes_[index].first_child; c != NOT_AVAILABLE;) {
      auto nxt = nodes_[c].right;
      move_to_before(c, index);
      c = nxt;
    }
    detach(index);
  }

  /**
   * @brief Replaces the children of this node with the given node.
   * The children are removed from this node's tree.
   * @param index
   * @param target cannot be the same as index
   */
  void replace_children(int index, int target) {
    for (auto c : get_children(index)) detach(c);
    move_to(target, index);
  }

  //================================================================================
  //    I/O
  //================================================================================
  std::string to_string(int root) const {
    std::stringstream ss;
    if (!is_valid(root)) {
      ss << "invalid(" << root << ")";
      return ss.str();
    }

    // DFS without recursive calls
    std::stack<int> stack;
    std::unordered_set<int> visited;

    stack.push(-1);
    stack.push(root);

    while (!stack.empty()) {
      auto p = stack.top();
      stack.pop();

      if (p != -1) {
        if (visited.find(p) != visited.end()) {
          // found a cycle
          return "cycle detected";
        }
        visited.insert(p);
        ss << "(" << nodes_[p].data;

        auto st = get_children(p);
        // add to the stack in reverse ordering
        for (auto it = st.rbegin(); it != st.rend(); ++it) {
          stack.push(-1);
          stack.push(*it);
        }
      } else {
        ss << ")";
      }
    }

    return ss.str();
  }

  //================================================================================
  //    Debugging
  //================================================================================
  /**
   * @brief Make sure that this data structure is not corrupt.
   *
   */
  void check_consistency() const {
    int num_alive = 0;
    for (int i = 0; i < static_cast<int>(capacity()); ++i) {
      if (!nodes_[i].is_alive()) continue;
      ++num_alive;
      // left & right connections
      if (nodes_[i].left != NOT_AVAILABLE && nodes_[nodes_[i].left].right != i) {
        throw std::runtime_error("left->right must be this");
      }
      if (nodes_[i].right != NOT_AVAILABLE && nodes_[nodes_[i].right].left != i) {
        throw std::runtime_error("right->left must be this");
      }
      // children
      if (static_cast<int>(get_children(i).size()) != nodes_[i].number_of_children()) {
        throw std::runtime_error("number of children does not match");
      }
      // parent
      if (nodes_[i].parent != NOT_AVAILABLE) {
        auto cs = get_children(nodes_[i].parent);
        if (!util::contains(cs, i)) throw std::runtime_error("parent must have this as a child");
      }
    }
    if (num_alive != static_cast<int>(size())) throw std::runtime_error("number of active nodes does not match");
  }

 private:
};
}  // namespace tree
}  // namespace ds
