#include <gtest/gtest.h>

#include "ds/graph/Graph.hpp"

using namespace std;
using namespace ds::graph;

template <typename Graph>
inline std::vector<std::vector<int>> create_adj(Graph const& g) {
  int n = g.number_of_nodes();
  std::vector<std::vector<int>> ret(n, std::vector<int>(n));

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (g.has_vertex(i) && g.has_vertex(j)) {
        ret[i][j] = g.has_edge(i, j) ? 1 : 0;
      } else {
        ret[i][j] = 2;
      }
    }
  }
  return ret;
}

std::vector<std::pair<int, int>> edges = {                   //
    {0, 1}, {1, 2}, {0, 3}, {0, 4}, {0, 5}, {1, 3}, {4, 1},  //
    {1, 5}, {3, 2}, {4, 2}, {5, 2}, {4, 6}, {6, 5}};

//
// GraphTest
//
TEST(GraphTest, BasicOperations) {
  auto G = Graph(7, edges, false);

  // properties
  EXPECT_EQ(G.number_of_nodes(), 7);
  EXPECT_EQ(G.number_of_edges(), 13);

  // queries
  EXPECT_TRUE(G.has_vertex(0));
  EXPECT_TRUE(G.has_vertex(6));
  EXPECT_FALSE(G.has_vertex(7));
  EXPECT_FALSE(G.has_vertex(-1));

  EXPECT_FALSE(G.has_edge(1, 1));
  EXPECT_TRUE(G.has_edge(1, 3));
  EXPECT_TRUE(G.has_edge(3, 1));
  EXPECT_FALSE(G.has_edge(0, 2));

  EXPECT_EQ(G.degree(0), 4);
  EXPECT_EQ(G.degree(4), 4);

  // adjacency matrix
  EXPECT_EQ(create_adj(G), vector<vector<int>>({
                               {0, 1, 0, 1, 1, 1, 0},
                               {1, 0, 1, 1, 1, 1, 0},
                               {0, 1, 0, 1, 1, 1, 0},
                               {1, 1, 1, 0, 0, 0, 0},
                               {1, 1, 1, 0, 0, 0, 1},
                               {1, 1, 1, 0, 0, 0, 1},
                               {0, 0, 0, 0, 1, 1, 0},
                           }));
}

TEST(GraphTest, BasicOperationsDense) {
  auto G = Graph(7, edges, true);

  // properties
  EXPECT_EQ(G.number_of_nodes(), 7);
  EXPECT_EQ(G.number_of_edges(), 13);

  // queries
  EXPECT_TRUE(G.has_vertex(0));
  EXPECT_TRUE(G.has_vertex(6));
  EXPECT_FALSE(G.has_vertex(7));
  EXPECT_FALSE(G.has_vertex(-1));

  EXPECT_FALSE(G.has_edge(1, 1));
  EXPECT_TRUE(G.has_edge(1, 3));
  EXPECT_TRUE(G.has_edge(3, 1));
  EXPECT_FALSE(G.has_edge(0, 2));

  EXPECT_EQ(G.degree(0), 4);
  EXPECT_EQ(G.degree(4), 4);

  // adjacency matrix
  EXPECT_EQ(create_adj(G), vector<vector<int>>({
                               {0, 1, 0, 1, 1, 1, 0},
                               {1, 0, 1, 1, 1, 1, 0},
                               {0, 1, 0, 1, 1, 1, 0},
                               {1, 1, 1, 0, 0, 0, 0},
                               {1, 1, 1, 0, 0, 0, 1},
                               {1, 1, 1, 0, 0, 0, 1},
                               {0, 0, 0, 0, 1, 1, 0},
                           }));
}
