#include <gtest/gtest.h>

#include "ds/tree/IntRootedForest.hpp"
#include "util/functional.hpp"

using namespace std;
using namespace ds::tree;

typedef std::vector<bool> VB;
typedef std::vector<int> VI;
typedef std::vector<std::pair<int, int>> VII;
typedef std::vector<std::string> VS;

// IntNode r0(0), r3(3), r10(10), r13(13);
// IntNode* nodes[20];

IntRootedForest<int> initialize_inttree() {
  // for (int i = 0; i < n; ++i) nodes[i] = new IntNode(i);
  //  0    3
  //       |
  //       4-5---1
  //       | |
  //       7 2-9
  //       |
  //       8-6

  // construction
  int n = 20;
  auto tree = IntRootedForest<int>();
  for (int i = 0; i < n; ++i) tree.create_node(i);

  VII relations = {{3, 1},   {3, 5},   {3, 4},   {5, 9},   {5, 2},   {4, 7},   {7, 6},   {7, 8},
                   {13, 11}, {13, 15}, {13, 14}, {15, 19}, {15, 12}, {14, 17}, {17, 16}, {17, 18}};

  for (auto &p : relations) tree.move_to(p.second, p.first);
  return tree;
}

template <typename T>
std::vector<T> collect(std::function<T(int)> f) {
  std::vector<T> ret;
  for (int i = 0; i < 10; ++i) ret.push_back(f(i));
  return ret;
}

TEST(IntRootedForestTest, IntRootedForest) {
  auto tree = initialize_inttree();

  EXPECT_EQ(tree.size(), 20);
  EXPECT_EQ(tree.capacity(), 20);
  EXPECT_EQ(tree.get_roots(), VI({0, 3, 10, 13}));

  // Node#is_root()
  EXPECT_EQ(collect<bool>([&](int i) { return tree[i].is_root(); }),
            VB({true, false, false, true, false, false, false, false, false, false}));

  // Node#has_parent()
  EXPECT_EQ(collect<bool>([&](int i) { return tree[i].has_parent(); }),
            VB({false, true, true, false, true, true, true, true, true, true}));

  // // Node#is_first_child()
  EXPECT_EQ(collect<bool>([&](int i) { return tree[i].is_first_child(); }),
            VB({false, false, true, false, true, false, false, true, true, false}));

  // // Node#is_last_child()
  EXPECT_EQ(collect<bool>([&](int i) { return tree[i].is_last_child(); }),
            VB({false, true, false, false, false, false, true, true, false, true}));

  // // Node#is_leaf()
  EXPECT_EQ(collect<bool>([&](int i) { return tree[i].is_leaf(); }),
            VB({true, true, true, false, false, false, true, false, true, true}));

  // // Node#has_child()
  EXPECT_EQ(collect<bool>([&](int i) { return tree[i].has_child(); }),
            VB({false, false, false, true, true, true, false, true, false, false}));

  // // Node#has_only_one_child()
  EXPECT_EQ(collect<bool>([&](int i) { return tree[i].has_only_one_child(); }),
            VB({false, false, false, false, true, false, false, false, false, false}));

  // // Node#is_alive()
  EXPECT_EQ(collect<bool>([&](int i) { return tree[i].is_alive(); }),
            VB({true, true, true, true, true, true, true, true, true, true}));

  // Node#number_of_children()
  {
    auto expected = VI({0, 0, 0, 3, 1, 2, 0, 2, 0, 0});
    EXPECT_EQ(collect<int>([&](int i) { return tree[i].number_of_children(); }), expected);
  }

  {  // IntRootedForest#to_string()
    auto expected = VS({"(0)", "(1)", "(2)", "(3(4(7(8)(6)))(5(2)(9))(1))", "(4(7(8)(6)))", "(5(2)(9))", "(6)",
                        "(7(8)(6))", "(8)", "(9)"});
    EXPECT_EQ(collect<string>([&](int i) { return tree.to_string(i); }), expected);
  }

  {  // IntRootedForest#get_children()
    vector<VI> expected = {
        {}, {}, {}, {4, 5, 1}, {7}, {2, 9}, {}, {8, 6}, {}, {},
    };
    EXPECT_EQ(collect<std::vector<int>>([&](int i) { return tree.get_children(i); }), expected);
  }

  {  // IntRootedForest#get_leaves()
    vector<VI> expected = {
        {0}, {1}, {2}, {1, 9, 2, 6, 8}, {6, 8}, {9, 2}, {6}, {6, 8}, {8}, {9},
    };
    EXPECT_EQ(collect<std::vector<int>>([&](int i) { return tree.get_leaves(i); }), expected);
  }

  {  // IntRootedForest#get_ancestors()
    vector<VI> expected = {
        {}, {3}, {5, 3}, {}, {3}, {3}, {7, 4, 3}, {4, 3}, {7, 4, 3}, {5, 3},
    };
    EXPECT_EQ(collect<std::vector<int>>([&](int i) { return tree.get_ancestors(i); }), expected);
  }
  tree.check_consistency();
}

TEST(IntRootedTreeTest, Detach) {
  auto tree = initialize_inttree();

  tree.detach(3);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(5(2)(9))(1))");
  EXPECT_EQ(tree[3].number_of_children(), 3);
  tree.detach(5);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(1))");
  EXPECT_EQ(tree.to_string(5), "(5(2)(9))");
  EXPECT_EQ(tree[3].number_of_children(), 2);
  tree.detach(4);
  EXPECT_EQ(tree.to_string(3), "(3(1))");
  EXPECT_EQ(tree.to_string(4), "(4(7(8)(6)))");
  EXPECT_EQ(tree[3].number_of_children(), 1);
  tree.detach(1);
  EXPECT_EQ(tree.to_string(3), "(3)");
  EXPECT_EQ(tree.to_string(1), "(1)");
  EXPECT_EQ(tree[3].number_of_children(), 0);
  tree.detach(0);
  EXPECT_EQ(tree.to_string(0), "(0)");

  EXPECT_EQ(tree.size(), 20);
  EXPECT_EQ(tree.capacity(), 20);
  EXPECT_EQ(tree.get_roots(), VI({0, 1, 3, 4, 5, 10, 13}));

  tree.check_consistency();
}

TEST(IntRootedTreeTest, Remove) {
  auto tree = initialize_inttree();

  tree.remove(2);
  tree.remove(9);
  tree.remove(5);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(1))");
  EXPECT_EQ(tree.size(), 17);
  EXPECT_EQ(tree.capacity(), 20);

  int i99 = tree.create_node(99);
  tree.move_to(i99, 3);
  EXPECT_EQ(tree.to_string(3), "(3(99)(4(7(8)(6)))(1))");
  EXPECT_EQ(tree.size(), 18);
  EXPECT_EQ(tree.capacity(), 20);

  int i98 = tree.create_node(98);
  tree.move_to(i98, i99);
  EXPECT_EQ(tree.to_string(3), "(3(99(98))(4(7(8)(6)))(1))");
  EXPECT_EQ(tree.size(), 19);
  EXPECT_EQ(tree.capacity(), 20);

  int i97 = tree.create_node(97);
  tree.move_to(i97, i98);
  EXPECT_EQ(tree.to_string(3), "(3(99(98(97)))(4(7(8)(6)))(1))");
  EXPECT_EQ(tree.size(), 20);
  EXPECT_EQ(tree.capacity(), 20);

  int i96 = tree.create_node(96);
  tree.move_to(i96, i98);
  EXPECT_EQ(tree.to_string(3), "(3(99(98(96)(97)))(4(7(8)(6)))(1))");
  EXPECT_EQ(tree.size(), 21);
  EXPECT_EQ(tree.capacity(), 21);

  tree.remove(i97);
  EXPECT_EQ(tree.to_string(3), "(3(99(98(96)))(4(7(8)(6)))(1))");
  EXPECT_EQ(tree.size(), 20);
  EXPECT_EQ(tree.capacity(), 21);

  EXPECT_EQ(tree.get_roots(), VI({0, 3, 10, 13}));
  tree.remove(0);
  EXPECT_EQ(tree.get_roots(), VI({3, 10, 13}));

  tree.check_consistency();
}

TEST(IntRootedTreeTest, Swap) {
  auto tree = initialize_inttree();

  tree.swap(5, 15);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(15(12)(19))(1))");
  EXPECT_EQ(tree.to_string(13), "(13(14(17(18)(16)))(5(2)(9))(11))");

  tree.swap(4, 11);
  EXPECT_EQ(tree.to_string(3), "(3(11)(15(12)(19))(1))");
  EXPECT_EQ(tree.to_string(13), "(13(14(17(18)(16)))(5(2)(9))(4(7(8)(6))))");

  tree.detach(7);
  tree.swap(1, 7);
  EXPECT_EQ(tree.to_string(3), "(3(11)(15(12)(19))(7(8)(6)))");

  tree.swap(7, 1);
  tree.swap(1, 7);
  EXPECT_EQ(tree.to_string(3), "(3(11)(15(12)(19))(7(8)(6)))");

  // tree.detach(7);
  // tree.swap(7, 7);
  // EXPECT_EQ(tree.to_string(7), "(7(8)(6))");
  tree.check_consistency();
}

TEST(IntRootedTreeTest, Replace) {
  auto tree = initialize_inttree();

  tree.replace(3, 5);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(1))");
  EXPECT_EQ(tree.to_string(5), "(5(2)(9))");
  tree.check_consistency();
}

TEST(IntRootedTreeTest, MoveToBefore) {
  auto tree = initialize_inttree();

  tree.move_to_before(15, 5);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(15(12)(19))(5(2)(9))(1))");

  tree.move_to_before(11, 1);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(15(12)(19))(5(2)(9))(11)(1))");

  tree.move_to_before(14, 4);
  EXPECT_EQ(tree.to_string(3), "(3(14(17(18)(16)))(4(7(8)(6)))(15(12)(19))(5(2)(9))(11)(1))");

  tree.move_to_before(11, 4);
  EXPECT_EQ(tree.to_string(3), "(3(14(17(18)(16)))(11)(4(7(8)(6)))(15(12)(19))(5(2)(9))(1))");
  tree.check_consistency();
}

TEST(IntRootedTreeTest, MoveToAfter) {
  auto tree = initialize_inttree();

  tree.move_to_after(15, 5);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(5(2)(9))(15(12)(19))(1))");

  tree.move_to_after(11, 1);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(5(2)(9))(15(12)(19))(1)(11))");

  tree.move_to_after(14, 4);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(14(17(18)(16)))(5(2)(9))(15(12)(19))(1)(11))");

  tree.move_to_after(11, 4);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(11)(14(17(18)(16)))(5(2)(9))(15(12)(19))(1))");
  tree.check_consistency();
}

TEST(IntRootedTreeTest, MakeFirstChild) {
  auto tree = initialize_inttree();

  tree.make_first_child(3);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(5(2)(9))(1))");

  tree.make_first_child(4);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(5(2)(9))(1))");

  tree.make_first_child(5);
  EXPECT_EQ(tree.to_string(3), "(3(5(2)(9))(4(7(8)(6)))(1))");

  tree.make_first_child(1);
  EXPECT_EQ(tree.to_string(3), "(3(1)(5(2)(9))(4(7(8)(6))))");

  tree.make_first_child(2);
  EXPECT_EQ(tree.to_string(3), "(3(1)(5(2)(9))(4(7(8)(6))))");

  tree.make_first_child(9);
  EXPECT_EQ(tree.to_string(3), "(3(1)(5(9)(2))(4(7(8)(6))))");

  tree.make_first_child(7);
  EXPECT_EQ(tree.to_string(3), "(3(1)(5(9)(2))(4(7(8)(6))))");
  tree.check_consistency();
}

TEST(IntRootedTreeTest, AddChildrenFrom) {
  auto tree = initialize_inttree();

  tree.add_children_from(3, 13);
  EXPECT_EQ(tree.to_string(3), "(3(14(17(18)(16)))(15(12)(19))(11)(4(7(8)(6)))(5(2)(9))(1))");
  EXPECT_EQ(tree.to_string(13), "(13)");
  EXPECT_EQ(tree[3].number_of_children(), 6);
  EXPECT_EQ(tree[13].number_of_children(), 0);

  tree.add_children_from(3, 1);
  EXPECT_EQ(tree.to_string(3), "(3(14(17(18)(16)))(15(12)(19))(11)(4(7(8)(6)))(5(2)(9))(1))");
  EXPECT_EQ(tree[3].number_of_children(), 6);
  EXPECT_EQ(tree[1].number_of_children(), 0);

  tree.add_children_from(3, 4);
  EXPECT_EQ(tree.to_string(3), "(3(7(8)(6))(14(17(18)(16)))(15(12)(19))(11)(4)(5(2)(9))(1))");
  EXPECT_EQ(tree[3].number_of_children(), 7);
  EXPECT_EQ(tree[4].number_of_children(), 0);

  tree.add_children_from(4, 5);
  EXPECT_EQ(tree.to_string(3), "(3(7(8)(6))(14(17(18)(16)))(15(12)(19))(11)(4(2)(9))(5)(1))");
  tree.check_consistency();
}

TEST(IntRootedTreeTest, ReplaceByChildren) {
  auto tree = initialize_inttree();

  tree.replace_by_children(5);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(2)(9)(1))");
  EXPECT_EQ(tree[3].number_of_children(), 4);

  tree.replace_by_children(4);
  EXPECT_EQ(tree.to_string(3), "(3(7(8)(6))(2)(9)(1))");
  EXPECT_EQ(tree[3].number_of_children(), 4);

  tree.replace_by_children(7);
  EXPECT_EQ(tree.to_string(3), "(3(8)(6)(2)(9)(1))");
  EXPECT_EQ(tree[3].number_of_children(), 5);

  tree.replace_by_children(1);
  EXPECT_EQ(tree.to_string(3), "(3(8)(6)(2)(9))");
  EXPECT_EQ(tree[3].number_of_children(), 4);
  tree.check_consistency();
}

TEST(IntRootedTreeTest, ReplaceChildren) {
  auto tree = initialize_inttree();

  tree.replace_children(5, 15);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(5(15(12)(19)))(1))");

  tree.replace_children(1, 0);
  EXPECT_EQ(tree.to_string(3), "(3(4(7(8)(6)))(5(15(12)(19)))(1(0)))");

  tree.replace_children(3, 1);
  EXPECT_EQ(tree.to_string(3), "(3(1(0)))");
  tree.check_consistency();
}
