#include <gtest/gtest.h>

#include <set>

#include "tww/ds/set/ArrayBitset.hpp"
#include "tww/util/Random.hpp"

using namespace tww::ds;

TEST(ArrayBitsetTest, BasicOperations) {
  auto s = ARRAY_BITSET(2000);
  EXPECT_TRUE(s.empty());

  s.set(0);
  EXPECT_FALSE(s.empty());
  EXPECT_TRUE(s.get(0));
  EXPECT_FALSE(s.get(1));
  EXPECT_EQ(s.size(), 1);
  EXPECT_EQ(s.to_vector(), std::vector<int>({0}));

  s.reset(0);
  EXPECT_TRUE(s.empty());
  EXPECT_FALSE(s.get(0));
  EXPECT_FALSE(s.get(1));
  EXPECT_EQ(s.size(), 0);
  EXPECT_EQ(s.to_vector(), std::vector<int>());

  s.set(1024);
  s.set(1025);
  s.set(1023);
  s.set(1020);
  s.set(1024);
  s.set(1020);
  s.set(1019);
  EXPECT_FALSE(s.empty());
  EXPECT_TRUE(s.get(1019));
  EXPECT_TRUE(s.get(1020));
  EXPECT_FALSE(s.get(1021));
  EXPECT_FALSE(s.get(1022));
  EXPECT_TRUE(s.get(1023));
  EXPECT_TRUE(s.get(1024));
  EXPECT_TRUE(s.get(1025));
  EXPECT_EQ(s.size(), 5);

  s.reset(1024);
  EXPECT_FALSE(s.get(1024));
  EXPECT_EQ(s.size(), 4);

  s.clear();
  EXPECT_TRUE(s.empty());
  EXPECT_FALSE(s.get(1019));
  EXPECT_EQ(s.size(), 0);
}

TEST(ArrayBitsetTest, RandomInput) {
  tww::util::Random rand(12345);

  auto b = ARRAY_BITSET(10000);
  std::set<int> s;

  for (int t = 0; t < 10; ++t) {
    b.clear();
    s.clear();

    for (int i = 0; i < 100; ++i) {
      int x = rand.randint(0, 10000);
      int y = rand.randint(5000, 10000);
      b.set(x);
      EXPECT_TRUE(b.get(x));
      b.reset(y);
      EXPECT_FALSE(b.get(y));

      s.insert(x);
      s.erase(y);

      EXPECT_EQ(b.size(), s.size());
      EXPECT_EQ(b.size(), b.to_vector().size());

      auto b2 = b;
      auto s2 = s;
      EXPECT_EQ(b2.size(), s2.size());
      EXPECT_EQ(b2.size(), b2.to_vector().size());
    }
  }
}
