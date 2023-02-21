#include "MDSolver.hpp"

namespace modular {
namespace compute {
namespace impl {

// split directions: left or right
std::vector<SplitDirection> const DIRS = {SplitDirection::LEFT, SplitDirection::RIGHT};

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
static bool is_root_operator(CompTree const &tree, int index) {
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
static void add_split_mark(CompTree &tree, int index, SplitDirection split_type, bool should_recurse, util::Profiler *prof) {
  if (!tree[index].data.is_split_marked(split_type)) {
    auto p = tree[index].parent;  // must be a valid node
    // increment the counter if the parent is an operation node
    if (tree[p].data.is_operation_node()) tree[p].data.increment_num_split_children(split_type);
    tree[index].data.set_split_mark(split_type);
  }

  if (!should_recurse || tree[index].data.op_type != Operation::PRIME) return;

  // split type is already set to all children
  if (tree[index].number_of_children() == tree[index].data.get_num_split_children(split_type)) {
    // PROF(util::pcount(prof, "add_split_mark(): early return"));
    return;
  }

  // PROF(util::pcount(prof, "add_split_mark(): proc child"));
  for (auto c = tree[index].first_child; tree.is_valid(c); c = tree[c].right) {
    if (!tree[c].data.is_split_marked(split_type)) {
      tree[index].data.increment_num_split_children(split_type);
      tree[c].data.set_split_mark(split_type);
    }
  }
}

/**
 * @brief Adds the given mark to all of this node's ancestors.
 * @param split_type mark to be added
 */
static void mark_ancestors_by_split(CompTree &tree, int index, SplitDirection split_type, util::Profiler *prof) {
  for (auto p = tree[index].parent;; p = tree[p].parent) {
    if (tree[p].data.is_problem_node()) break;
    if (tree[p].data.is_split_marked(split_type)) {
      // split type is already set to p but we need to take care of its children
      add_split_mark(tree, p, split_type, true, prof);
      break;
    }
    add_split_mark(tree, p, split_type, true, prof);
  }
}

//================================================================================
//    Get max subtrees
//================================================================================
static bool is_parent_fully_charged(CompTree const &tree, int x) {
  if (is_root_operator(tree, x)) return false;
  auto p = tree[x].parent;
  return tree[p].number_of_children() == tree[p].data.number_of_marks();
}

/**
 * @brief Finds the set of maximal subtrees such that the leaves of
 * each subtree are subsets of the given leaf set.
 *
 * @param leaves
 * @return std::list<NodeP>
 */
static std::vector<int> get_max_subtrees(CompTree &tree, VI const &leaves) {
  std::vector<int> full_charged(leaves);
  std::vector<int> charged;

  // charging
  for (std::size_t i = 0; i < full_charged.size(); ++i) {
    auto x = full_charged[i];
    if (is_root_operator(tree, x)) continue;

    auto p = tree[x].parent;
    if (!tree[p].data.is_marked()) charged.push_back(p);
    tree[p].data.add_mark();

    if (tree[p].data.num_marks == tree[p].number_of_children()) {
      // fully charged
      full_charged.push_back(p);
    }
  }

  // discharging
  std::vector<int> ret;
  for (auto x : full_charged) {
    if (!is_parent_fully_charged(tree, x)) ret.push_back(x);
  }
  for (auto x : charged) tree[x].data.clear_marks();
  return ret;
}

//================================================================================
//    Group sibling nodes
//================================================================================
std::vector<std::pair<int, bool>> group_sibling_nodes(CompTree &tree, std::vector<int> const &nodes) {
  std::vector<int> parents;
  std::vector<std::pair<int, bool>> sibling_groups;

  for (auto node : nodes) {
    if (is_root_operator(tree, node)) {
      // (1) the roots of trees
      sibling_groups.push_back({node, false});
    } else {
      tree.make_first_child(node);
      auto p = tree[node].parent;

      if (!tree[p].data.is_marked()) parents.push_back(p);
      tree[p].data.add_mark();
    }
  }

  for (auto p : parents) {
    // there must be at least one mark
    auto c = tree[p].first_child;
    auto num_marks = tree[p].data.number_of_marks();

    if (num_marks == 1) {
      // (2) the non-root nodes without siblings
      sibling_groups.push_back({c, false});
    } else {
      // (3) group sibling nodes as children of a new node inserted in their place.
      auto grouped_children = tree.create_node(tree[p].data);
      tree[grouped_children].data.clear_marks();

      for (auto st : DIRS) {
        if (tree[grouped_children].data.is_split_marked(st)) tree[p].data.increment_num_split_children(st);
      }

      auto c = tree[p].first_child;
      for (int i = 0; i < num_marks; ++i) {
        auto nxt = tree[c].right;
        tree.move_to(c, grouped_children);

        for (auto st : DIRS) {
          if (tree[c].data.is_split_marked(st)) {
            tree[p].data.decrement_num_split_children(st);
            tree[grouped_children].data.increment_num_split_children(st);
          }
        }
        c = nxt;
      }
      tree.move_to(grouped_children, p);

      sibling_groups.push_back({grouped_children, tree[grouped_children].data.op_type == Operation::PRIME});
    }
    tree[p].data.clear_marks();
  }

  // TRACE("return: %s\n", cstr(sibling_groups));
  return sibling_groups;
}

//================================================================================
//    Subroutine
//================================================================================
static SplitDirection get_split_type(CompTree &tree, int index, VertexID refiner, VertexID pivot) {
  auto pivot_tn = tree[pivot].data.tree_number;
  auto refiner_tn = tree[refiner].data.tree_number;
  auto current = tree[index].data.tree_number;
  return current < pivot_tn || refiner_tn < current ? SplitDirection::LEFT : SplitDirection::RIGHT;
}

static void refine_one_node(CompTree &tree, int index, SplitDirection split_type, bool new_prime, util::Profiler *prof) {
  TRACE("refining tree=%s, index=%d, split_type=%d, new_prime=%d", tree.to_string(index).c_str(), index, split_type, new_prime);
  if (is_root_operator(tree, index)) return;

  auto p = tree[index].parent;
  int new_sibling = -1;

  if (is_root_operator(tree, p)) {
    // PROF(util::pstart(prof, "refine_one_node: root", 0));
    // parent is a root; must split there
    if (split_type == SplitDirection::LEFT) {
      tree.move_to_before(index, p);
    } else {
      tree.move_to_after(index, p);
    }

    for (auto st : DIRS) {
      if (tree[index].data.is_split_marked(st)) tree[p].data.decrement_num_split_children(st);
    }

    new_sibling = p;

    if (tree[p].has_only_one_child()) {
      tree.replace_by_children(p);
      tree.remove(p);
      new_sibling = -1;
    }
    // PROF(util::pstop(prof, "refine_one_node: root", 0));
  } else if (tree[p].data.op_type != Operation::PRIME) {
    // PROF(util::pstart(prof, "refine_one_node: non-root", 0));
    // parent is not a root or PRIME
    auto replacement = tree.create_node(tree[p].data);
    tree.replace(p, replacement);
    tree.move_to(index, replacement);
    tree.move_to(p, replacement);
    new_sibling = p;

    for (auto st : DIRS) {
      if (tree[index].data.is_split_marked(st)) {
        tree[p].data.decrement_num_split_children(st);
        tree[replacement].data.increment_num_split_children(st);
      }
      if (tree[p].data.is_split_marked(st)) tree[replacement].data.increment_num_split_children(st);
    }
    // PROF(util::pstop(prof, "refine_one_node: non-root", 0));
  }

  // PROF(util::pstart(prof, "add_split_mark()", 1));
  add_split_mark(tree, index, split_type, new_prime, prof);
  // PROF(util::pstop(prof, "add_split_mark()", 1));

  // PROF(util::pstart(prof, "mark_ancestors_by_split()"));
  mark_ancestors_by_split(tree, index, split_type, prof);
  // PROF(util::pstop(prof, "mark_ancestors_by_split()"));

  if (new_sibling >= 0) {
    // non-prime or a new root; safe to set should_recurse=true
    // PROF(util::pstart(prof, "add_split_mark()", 2));
    add_split_mark(tree, new_sibling, split_type, true, prof);
    // PROF(util::pstop(prof, "add_split_mark()", 2));
  }
}

static void refine_with(CompTree &tree, VVV const &alpha_list, VertexID refiner, VertexID pivot, util::Profiler *prof) {
  // PROF(util::pstart(prof, "get_max_subtrees()"))
  auto subtree_roots = get_max_subtrees(tree, alpha_list[refiner]);
  // PROF(util::pstop(prof, "get_max_subtrees()"))

  // PROF(util::pstart(prof, "group_sibling_nodes()"))
  auto sibling_groups = group_sibling_nodes(tree, subtree_roots);
  // PROF(util::pstop(prof, "group_sibling_nodes()"))

  TRACE("alpha[%d]: %s", refiner, util::to_string(alpha_list[refiner]).c_str());
  TRACE("subtree_roots: %s", util::to_string(subtree_roots).c_str());
  TRACE("sibling_groups: %s, tree=%s", util::to_string(sibling_groups).c_str(), tree.to_string(tree[pivot].parent).c_str());

  // PROF(util::pstart(prof, "refine_with: main loop"))
  for (auto &x : sibling_groups) {
    // PROF(util::pstart(prof, "get_split_type()"))
    auto split_type = get_split_type(tree, x.first, refiner, pivot);
    // PROF(util::pstop(prof, "get_split_type()"))

    // PROF(util::pstart(prof, "refine_one_node()"))
    refine_one_node(tree, x.first, split_type, x.second, prof);
    // PROF(util::pstop(prof, "refine_one_node()"))
  }
  // PROF(util::pstop(prof, "refine_with: main loop"))
}

//================================================================================
//    Refinement
//================================================================================
void refine(CompTree &tree, VVV const &alpha_list, int prob, std::vector<int> &leaves, util::Profiler *prof) {
  TRACE("start: %s", tree.to_string(prob).c_str());
  // PROF(util::pstart(prof, "refinement:refine()"))

  // PROF(util::pstart(prof, "refinement:number_by_comp()"))
  number_by_comp(tree, prob);
  // PROF(util::pstop(prof, "refinement:number_by_comp()"))

  // PROF(util::pstart(prof, "refinement:number_by_tree()"))
  number_by_tree(tree, prob);
  // PROF(util::pstop(prof, "refinement:number_by_tree()"))

  // PROF(util::pstart(prof, "refinement:refine_with()"));
  for (auto v : leaves) {
    refine_with(tree, alpha_list, v, tree[prob].data.vertex, prof);
    TRACE("refined at %d: tree=%s", v, tree.to_string(prob).c_str())
  }
  // PROF(util::pstop(prof, "refinement:refine_with()"));

  // PROF(util::pstop(prof, "refinement:refine()"))
  TRACE("finish: %s", tree.to_string(prob).c_str());
}

}  // namespace impl
}  // namespace compute
}  // namespace modular
