#include <gtest/gtest.h>

#include "modular/MDTree.hpp"

using namespace std;
using namespace ds::graph;
using namespace modular;

TEST(MDTreeTest, MDTree) {
  vector<pair<int, int>> edges = {
      {0, 2}, {0, 3}, {0, 6}, {0, 7}, {1, 6}, {2, 3}, {2, 4}, {2, 5},
      {2, 7}, {3, 4}, {3, 5}, {4, 5}, {4, 6}, {4, 7}, {5, 6}, {5, 7},
  };
  Graph g(8, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(P(U(0)(J(4)(5)))(1)(J(2)(U(3)(7)))(6))");
  EXPECT_EQ(t.get_root(), 12);
  EXPECT_EQ(t.get_vertex(0), 0);
  EXPECT_EQ(t.get_vertex(1), 4);
  EXPECT_EQ(t.get_vertex(2), 5);
  EXPECT_EQ(t.get_vertex(3), 1);
  EXPECT_EQ(t.modular_width(), 4);

  std::vector<std::pair<int, int>> bounds, expected = {
                                               {0, 8},                          // P
                                               {0, 3}, {3, 4}, {4, 7}, {7, 8},  // U 1 J 6
                                               {0, 1}, {1, 3}, {4, 5}, {5, 7},  // 0J  2U
                                               {1, 2}, {2, 3}, {5, 6}, {6, 7}   //  45  37
                                           };
  for (auto nd : t.get_tree().bfs_nodes(t.get_root())) {
    bounds.push_back({t.get_tree()[nd].data.vertices_begin, t.get_tree()[nd].data.vertices_end});
  }

  EXPECT_EQ(bounds, expected);
}

TEST(MDTreeTest, MDTree2) {
  vector<pair<int, int>> edges = {
      {1, 2}, {2, 3}, {3, 4}, {5, 6}, {6, 7}, {7, 8}, {0, 1}, {0, 2}, {0, 3}, {0, 4},
      {9, 5}, {9, 6}, {9, 7}, {9, 8}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {2, 5}, {2, 6},
      {2, 7}, {2, 8}, {3, 5}, {3, 6}, {3, 7}, {3, 8}, {4, 5}, {4, 6}, {4, 7}, {4, 8},
  };
  Graph g(10, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(P(0)(P(1)(2)(3)(4))(P(5)(6)(7)(8))(9))");
  EXPECT_EQ(t.modular_width(), 4);
}

TEST(MDTreeTest, MDTree16) {
  vector<pair<int, int>> edges = {
      {0, 1},  {0, 2},  {0, 3},  {1, 4},  {1, 5},  {1, 6},  {2, 7},  {2, 8},  {2, 9},  {3, 10}, {3, 11}, {3, 12},
      {4, 13}, {4, 14}, {4, 15}, {5, 16}, {5, 17}, {5, 18}, {6, 19}, {6, 20}, {6, 21}, {7, 22}, {7, 23}, {7, 24},
  };
  Graph g(25, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(P(0)(1)(2)(3)(4)(5)(6)(7)(U(8)(9))(U(10)(11)(12))(U(13)(14)(15))(U(16)(17)(18))(U(19)(20)(21))(U(22)(23)(24)))");
  EXPECT_EQ(t.modular_width(), 4);
}
