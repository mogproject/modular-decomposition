#include <gtest/gtest.h>

#include "modular/MDTree.hpp"

using namespace std;
using namespace ds::graph;
using namespace modular;

Graph complement(Graph const& g) {
  int n = g.number_of_nodes();
  Graph h(n);

  for (int i = 0; i < n - 1; ++i) {
    for (int j = i + 1; j < n; ++j) {
      if (!g.has_edge(i, j)) h.add_edge(i, j);
    }
  }
  return h;
}

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

TEST(MDTreeTest, MDTree8) {
  vector<pair<int, int>> edges = {{0, 2},  {0, 5},  {1, 2},  {1, 3}, {2, 4},  {3, 5},  {3, 12},
                                  {5, 13}, {6, 10}, {6, 13}, {7, 8}, {7, 11}, {9, 13}, {11, 13}};
  Graph g(14, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(P(0)(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12)(13))");
  EXPECT_EQ(t.modular_width(), 14);

  MDTree t2(complement(g), true);
  EXPECT_EQ(t2.to_string(), "(P(0)(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12)(13))");
  EXPECT_EQ(t2.modular_width(), 14);
}

TEST(MDTreeTest, MDTree9) {
  vector<pair<int, int>> edges = {{0, 7}, {1, 4}, {2, 4}, {2, 7}, {2, 8}, {4, 5}};
  Graph g(9, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(U(P(0)(U(1)(5))(2)(4)(7)(8))(3)(6))");
  EXPECT_EQ(t.modular_width(), 6);

  MDTree t2(complement(g), true);
  EXPECT_EQ(t2.to_string(), "(J(P(0)(J(1)(5))(2)(4)(7)(8))(3)(6))");
  EXPECT_EQ(t2.modular_width(), 6);
}

TEST(MDTreeTest, MDTree10) {
  vector<pair<int, int>> edges = {{0, 8}, {1, 6}, {1, 7}, {4, 8}, {5, 7}, {6, 8}, {6, 9}, {8, 9}, {9, 11}};
  Graph g(12, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(U(P(U(0)(4))(1)(5)(6)(7)(8)(9)(11))(2)(3)(10))");
  EXPECT_EQ(t.modular_width(), 8);

  MDTree t2(complement(g), true);
  EXPECT_EQ(t2.to_string(), "(J(P(J(0)(4))(1)(5)(6)(7)(8)(9)(11))(2)(3)(10))");
  EXPECT_EQ(t2.modular_width(), 8);
}

TEST(MDTreeTest, MDTree11) {
  vector<pair<int, int>> edges = {{0, 5}, {1, 3}, {1, 8}, {3, 8}, {4, 9}, {7, 8}, {8, 9}};
  Graph g(11, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(U(J(0)(5))(P(U(J(1)(3))(7))(4)(8)(9))(2)(6)(10))");
  EXPECT_EQ(t.modular_width(), 4);

  MDTree t2(complement(g), true);
  EXPECT_EQ(t2.to_string(), "(J(U(0)(5))(P(J(U(1)(3))(7))(4)(8)(9))(2)(6)(10))");
  EXPECT_EQ(t2.modular_width(), 4);
}

TEST(MDTreeTest, MDTree12) {
  vector<pair<int, int>> edges = {{0, 10}, {0, 13}, {1, 3}, {1, 10}, {2, 13}, {3, 9},  {3, 10},
                                  {3, 13}, {4, 7},  {5, 9}, {5, 10}, {9, 10}, {11, 13}};
  Graph g(14, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(U(P(0)(1)(U(2)(11))(3)(5)(9)(10)(13))(J(4)(7))(6)(8)(12))");
  EXPECT_EQ(t.modular_width(), 8);

  MDTree t2(complement(g), true);
  EXPECT_EQ(t2.to_string(), "(J(P(0)(1)(J(2)(11))(3)(5)(9)(10)(13))(U(4)(7))(6)(8)(12))");
  EXPECT_EQ(t2.modular_width(), 8);
}

TEST(MDTreeTest, MDTree13) {
  vector<pair<int, int>> edges = {{0, 3}, {0, 7}, {1, 3}, {1, 6}, {2, 3}, {2, 4},
                                  {2, 5}, {3, 4}, {3, 6}, {3, 7}, {4, 5}, {4, 6}};
  Graph g(8, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(P(J(0)(7))(1)(2)(3)(4)(5)(6))");
  EXPECT_EQ(t.modular_width(), 7);

  MDTree t2(complement(g), true);
  EXPECT_EQ(t2.to_string(), "(P(U(0)(7))(1)(2)(3)(4)(5)(6))");
  EXPECT_EQ(t2.modular_width(), 7);
}

TEST(MDTreeTest, MDTree14) {
  vector<pair<int, int>> edges = {{0, 12}, {1, 2},  {1, 3},  {1, 9},  {3, 4},  {3, 5},  {3, 6},
                                  {3, 8},  {3, 10}, {3, 11}, {3, 12}, {4, 11}, {5, 10}, {6, 11},
                                  {6, 12}, {7, 12}, {8, 9},  {8, 12}, {9, 10}, {9, 12}, {10, 11}};
  Graph g(13, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(P(U(0)(7))(1)(2)(3)(4)(5)(6)(8)(9)(10)(11)(12))");
  EXPECT_EQ(t.modular_width(), 12);

  MDTree t2(complement(g), true);
  EXPECT_EQ(t2.to_string(), "(P(J(0)(7))(1)(2)(3)(4)(5)(6)(8)(9)(10)(11)(12))");
  EXPECT_EQ(t2.modular_width(), 12);
}

TEST(MDTreeTest, MDTree15) {
  vector<pair<int, int>> edges = {{0, 1},  {0, 4},  {0, 7}, {0, 8},  {0, 12}, {1, 7},  {1, 9},  {1, 10},
                                  {1, 11}, {2, 4},  {3, 5}, {3, 6},  {3, 9},  {3, 11}, {3, 13}, {4, 12},
                                  {5, 12}, {6, 13}, {7, 8}, {8, 12}, {9, 11}, {9, 13}};
  Graph g(14, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(), "(P(0)(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12)(13))");
  EXPECT_EQ(t.modular_width(), 14);

  MDTree t2(complement(g), true);
  EXPECT_EQ(t2.to_string(), "(P(0)(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12)(13))");
  EXPECT_EQ(t2.modular_width(), 14);
}

TEST(MDTreeTest, MDTree16) {
  vector<pair<int, int>> edges = {
      {0, 1},  {0, 2},  {0, 3},  {1, 4},  {1, 5},  {1, 6},  {2, 7},  {2, 8},  {2, 9},  {3, 10}, {3, 11}, {3, 12},
      {4, 13}, {4, 14}, {4, 15}, {5, 16}, {5, 17}, {5, 18}, {6, 19}, {6, 20}, {6, 21}, {7, 22}, {7, 23}, {7, 24},
  };
  Graph g(25, edges);

  MDTree t(g, true);
  EXPECT_EQ(t.to_string(),
            "(P(0)(1)(2)(3)(4)(5)(6)(7)(U(8)(9))(U(10)(11)(12))(U(13)(14)(15))"
            "(U(16)(17)(18))(U(19)(20)(21))(U(22)(23)(24)))");
  EXPECT_EQ(t.modular_width(), 14);

  MDTree t2(complement(g), true);
  EXPECT_EQ(t2.to_string(), 
            "(P(0)(1)(2)(3)(4)(5)(6)(7)(J(8)(9))(J(10)(11)(12))(J(13)(14)(15))"
            "(J(16)(17)(18))(J(19)(20)(21))(J(22)(23)(24)))");
  EXPECT_EQ(t2.modular_width(), 14);
}
