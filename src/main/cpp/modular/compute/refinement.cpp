#include "MDSolver.hpp"

namespace modular {
namespace compute {
namespace impl {

//================================================================================
//    Set up
//================================================================================
/**
 * @brief Updates the component number for each node in tree given problem subtree.
 *
 * @param tree tree
 * @param prob problem root
 */
static void number_by_comp(CompTree &tree, int prob) {
  int comp_number = 0;
  int pivot = tree[prob].data.vertex;
  Operation op_type = Operation::SERIES;

  for (auto c : tree.get_children(prob)) {
    if (c == pivot) op_type = Operation::PARALLEL;

    if (tree[c].data.op_type == op_type) {
      for (auto x : tree.get_children(c)) {
        for (auto y : tree.dfs_reverse_preorder_nodes(x)) tree[y].data.comp_number = comp_number;
        ++comp_number;
      }
    } else {
      for (auto y : tree.dfs_reverse_preorder_nodes(c)) tree[y].data.comp_number = comp_number;
      ++comp_number;
    }
  }
}

static void number_by_tree(CompTree &tree, int prob) {
  int tree_number = 0;
  for (auto c : tree.get_children(prob)) {
    for (auto y : tree.dfs_reverse_preorder_nodes(c)) tree[y].data.tree_number = tree_number;
    ++tree_number;
  }
}

//================================================================================
//    Utilities
//================================================================================
static bool is_root_operator(CompTree &tree, int index) {
  return tree[index].is_root() || !tree[tree[index].parent].data.is_operation_node();
}

//================================================================================
//    Marking split types
//================================================================================
/**
 * @brief Adds the given mark to the node and possibly its children.
 *
 * @param tree tree
 * @param index index
 * @param split_type split type
 * @param should_recurse true if it needs to recurse to children
 */
static void add_split_mark(CompTree &tree, int index, SplitDirection split_type, bool should_recurse) {
  tree[index].data.set_split_mark(split_type);

  if (should_recurse && tree[index].data.op_type == Operation::PRIME) {
    for (auto c : tree.get_children(index)) {
      if (!tree[c].data.is_split_marked(split_type)) tree[c].data.set_split_mark(split_type);
    }
  }
}

/**
 * @brief Adds the given mark to all of this node's ancestors.
 * @param split_type mark to be added
 */
static void mark_ancestors_by_split(CompTree &tree, int index, SplitDirection split_type) {
  for (auto p : tree.get_ancestors(index)) {
    if (tree[p].data.is_problem_node()) break;  // passed the operation root
    add_split_mark(tree, p, split_type, true);
  }
}

//================================================================================
//    Get max subtrees
//================================================================================
/**
 * @brief Finds the set of maximal subtrees such that the leaves of
 * each subtree are subsets of the given leaf set.
 *
 * @param leaves
 * @return std::list<NodeP>
 */
static std::vector<int> get_max_subtrees(CompTree &tree, VI const &leaves) {
  // charging
  std::map<int, int> num_charges;
  std::stack<int> stack;
  std::unordered_set<int> fully_charged;
  for (auto v : leaves) {
    fully_charged.insert(v);
    stack.push(v);
  }

  while (!stack.empty()) {
    auto x = stack.top();
    stack.pop();
    if (!is_root_operator(tree, x)) {
      auto p = tree[x].parent;
      ++num_charges[p];

      if (tree[p].number_of_children() == num_charges[p]) {
        // fully charged
        fully_charged.insert(p);

        // discharge children
        for (auto c: tree.get_children(p)) fully_charged.erase(c);

        stack.push(p);
      }
    }
  }

  return std::vector<int>(fully_charged.begin(), fully_charged.end());
}

// -----------------------original--------------------------------------------
// static std::list<int> get_max_subtrees(CompTree &tree, VI const &leaves) {
//   std::list<int> discharged;

//   // charging process
//   std::stack<int> stack;
//   for (auto v : leaves) stack.push(v);

//   while (!stack.empty()) {
//     auto x = stack.top();
//     stack.pop();
//     if (!is_root_operator(tree, x)) {
//       auto p = tree[x].parent;
//       tree[p].data.add_mark();
//       if (tree[p].data.num_marks == tree[p].number_of_children()) stack.push(p);
//     }
//     discharged.push_back(x);
//   }

//   // remove marks on all nodes
//   for (auto it = discharged.begin(); it != discharged.end(); ++it) {
//     tree[*it].data.clear_marks();
//     if (!is_root_operator(tree, *it)) {
//       auto p = tree[*it].parent;
//       if (tree[p].data.num_marks == tree[p].number_of_children()) {
//         it = discharged.erase(it);
//         --it;
//       } else {
//         tree[p].data.clear_marks();
//       }
//     }
//   }
//   return discharged;
// }
// -----------------------original--------------------------------------------

//================================================================================
//    Group sibling nodes
//================================================================================
std::vector<std::pair<int, bool>> group_sibling_nodes(CompTree &tree, std::vector<int> const &nodes) {
  std::map<int, int> num_marks;
  std::vector<int> parents;

  for (auto node : nodes) {
    ++num_marks[node];

    if (!is_root_operator(tree, node)) {
      tree.make_first_child(node);
      auto p = tree[node].parent;

      if (!util::contains(num_marks, p)) parents.push_back(p);
      ++num_marks[p];
    }
  }

  std::vector<std::pair<int, bool>> sibling_groups;

  // (1) the roots of trees
  for (auto node : nodes) {
    if (is_root_operator(tree, node)) {
      num_marks.erase(node);
      sibling_groups.push_back({node, false});
    }
  }

  for (auto p : parents) {
    // there must be at least one mark
    auto c = tree[p].first_child;

    if (num_marks[p] == 1) {
      // (2) the non-root nodes without siblings
      sibling_groups.push_back({c, false});
    } else {
      // (3) group sibling nodes as children of a new node inserted in their place.
      auto grouped_children = tree.create_node(tree[p].data);

      auto c = tree[p].first_child;
      for (int i = 0; i < num_marks[p]; ++i) {
        auto nxt = tree[c].right;
        tree.move_to(c, grouped_children);
        c = nxt;
      }
      tree.move_to(grouped_children, p);

      sibling_groups.push_back({grouped_children, tree[grouped_children].data.op_type == Operation::PRIME});
    }
  }

  // TRACE("return: %s\n", cstr(sibling_groups));
  // util::pstop(prof, "group_sibling_nodes()");
  return sibling_groups;
}

// -----------------------original--------------------------------------------
// std::vector<std::pair<int, bool>> group_sibling_nodes(CompTree &tree, std::list<int> const &nodes) {
//   // precondition: nodes/parents do not have marks
//   for (auto node : nodes) {
//     if (tree[node].data.is_marked()) throw std::invalid_argument("found unclean marks");
//     if (tree[node].has_parent() && tree[tree[node].parent].data.is_marked()) {
//       throw std::invalid_argument("found unclean marks");
//     }
//   }

//   std::vector<int> parents;
//   for (auto node : nodes) {
//     tree[node].data.add_mark();
//     if (!is_root_operator(tree, node)) {
//       tree.make_first_child(node);
//       auto p = tree[node].parent;
//       if (!tree[p].data.is_marked()) parents.push_back(p);
//       tree[p].data.add_mark();
//     }
//   }

//   std::vector<std::pair<int, bool>> sibling_groups;

//   // (1) the roots of trees
//   for (auto node : nodes) {
//     if (is_root_operator(tree, node)) {
//       tree[node].data.clear_marks();
//       sibling_groups.push_back({node, false});
//     }
//   }

//   for (auto p : parents) {
//     // there must be at least one mark
//     auto c = tree[p].first_child;

//     if (tree[p].data.num_marks == 1) {
//       // (2) the non-root nodes without siblings
//       tree[p].data.clear_marks();
//       tree[c].data.clear_marks();
//       sibling_groups.push_back({c, false});
//     } else {
//       // (3) group sibling nodes as children of a new node inserted in their place.
//       int nm = tree[p].data.num_marks;
//       tree[p].data.clear_marks();
//       auto grouped_children = tree.create_node(tree[p].data);

//       auto c = tree[p].first_child;
//       for (int i = 1; i < nm; ++i) {
//         auto nxt = tree[c].right;
//         tree.move_to(c, grouped_children);
//         c = nxt;
//       }
//       tree.move_to(grouped_children, p);

//       sibling_groups.push_back({grouped_children, tree[grouped_children].data.op_type == Operation::PRIME});
//     }
//   }

//   // TRACE("return: %s\n", cstr(sibling_groups));
//   // util::pstop(prof, "group_sibling_nodes()");
//   return sibling_groups;
// }
// -----------------------original--------------------------------------------

//================================================================================
//    Subroutine
//================================================================================
static SplitDirection get_split_type(CompTree &tree, int index, VertexID refiner, VertexID pivot) {
  auto pivot_tn = tree[pivot].data.tree_number;
  auto refiner_tn = tree[refiner].data.tree_number;
  auto current = tree[index].data.tree_number;
  return current < pivot_tn || refiner_tn < current ? SplitDirection::LEFT : SplitDirection::RIGHT;
}

static void refine_one_node(CompTree &tree, int index, SplitDirection split_type, bool new_prime) {
  TRACE("refining tree=%s, index=%d, split_type=%d, new_prime=%d", tree.to_string(index).c_str(), index, split_type, new_prime);
  if (is_root_operator(tree, index)) return;

  auto p = tree[index].parent;
  int new_sibling = -1;

  if (is_root_operator(tree, p)) {
    // parent is a root; must split there
    if (split_type == SplitDirection::LEFT) {
      tree.move_to_before(index, p);
    } else {
      tree.move_to_after(index, p);
    }
    new_sibling = p;

    if (tree[p].has_only_one_child()) {
      tree.replace_by_children(p);
      tree.remove(p);
      new_sibling = -1;
    }
  } else if (tree[p].data.op_type != Operation::PRIME) {
    // parent is not a root or PRIME
    auto replacement = tree.create_node(tree[p].data);
    tree.replace(p, replacement);
    tree.move_to(index, replacement);
    tree.move_to(p, replacement);
    new_sibling = p;
  }

  add_split_mark(tree, index, split_type, new_prime);
  mark_ancestors_by_split(tree, index, split_type);

  if (new_sibling >= 0) {
    // non-prime or a new root; safe to set should_recurse=true
    add_split_mark(tree, new_sibling, split_type, true);
  }
}

static void refine_with(CompTree &tree, VI const alpha_list[], VertexID refiner, VertexID pivot) {
  auto subtree_roots = get_max_subtrees(tree, alpha_list[refiner]);
  auto sibling_groups = group_sibling_nodes(tree, subtree_roots);

  TRACE("alpha[%d]: %s", refiner, util::to_string(alpha_list[refiner]).c_str());
  TRACE("subtree_roots: %s", util::to_string(subtree_roots).c_str());
  TRACE("sibling_groups: %s, tree=%s", util::to_string(sibling_groups).c_str(), tree.to_string(tree[pivot].parent).c_str());

  for (auto &x : sibling_groups) {
    auto split_type = get_split_type(tree, x.first, refiner, pivot);
    refine_one_node(tree, x.first, split_type, x.second);
  }
}

//================================================================================
//    Refinement
//================================================================================
void refine(CompTree &tree, VI const alpha_list[], int prob) {
  TRACE("start: %s", tree.to_string(prob).c_str());

  number_by_comp(tree, prob);
  number_by_tree(tree, prob);

  for (auto v : tree.get_leaves(prob)) {
    refine_with(tree, alpha_list, v, tree[prob].data.vertex);
    TRACE("refined at %d: tree=%s", v, tree.to_string(prob).c_str())
  }

  TRACE("finish: %s", tree.to_string(prob).c_str());
}

}  // namespace impl
}  // namespace compute
}  // namespace modular
