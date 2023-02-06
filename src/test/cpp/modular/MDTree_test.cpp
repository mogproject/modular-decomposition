#include <gtest/gtest.h>

#include "modular/MDTree.hpp"

using namespace std;
using namespace ds::graph;
using namespace modular;

TEST(MDTreeTest, MDTree) {
  vector<pair<int, int>> edges = {
    {0, 2}, {0, 3}, {0, 6}, {0, 7},
    {1, 6},
    {2, 3}, {2, 4}, {2, 5}, {2, 7},
    {3, 4}, {3, 5},
    {4, 5}, {4, 6}, {4, 7},
    {5, 6}, {5, 7},
  };
  Graph g(8, edges);

  MDTree t(g);
  EXPECT_EQ(t.to_string(), "(P(1)(6)(J(2)(U(3)(7)))(U(J(4)(5))(0)))");
  EXPECT_EQ(t.get_root(), 12);
  // EXPECT_EQ(t.get_vertex(0), 1);
  // EXPECT_EQ(t.get_vertex(1), 6);
  // EXPECT_EQ(t.get_vertex(2), 2);
  // EXPECT_EQ(t.get_vertex(3), 3);
  EXPECT_EQ(t.modular_width(), 4);

  // g = to_undirected(load_pace_2022("../../data/2022/small/003.graph"));
  // t = MDTree(g);
  // EXPECT_EQ(t.to_string(), "(P(16)(J(12)(U(13)(14)))(15)(U(J(10)(11))(9)(1)(P(17)(U(J(3)(4))(2))(J(5)(6)(7))(8)))(0))");
  // EXPECT_EQ(t.get_root(), 26);
  // EXPECT_EQ(t.get_vertex(0), 16);
  // EXPECT_EQ(t.get_vertex(1), 12);
  // EXPECT_EQ(t.get_vertex(2), 13);
  // EXPECT_EQ(t.get_vertex(3), 14);
  // EXPECT_EQ(t.modular_width(), 5);
}
