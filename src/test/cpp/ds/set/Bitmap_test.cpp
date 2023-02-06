#include <gtest/gtest.h>

#include "ds/set/Bitmap.hpp"

using namespace std;
using namespace ds;

TEST(BitmapTest, BitmapTest) {
  Bitmap b(700);
  EXPECT_EQ(b.to_vector(), std::vector<int>({}));
  b |= 0;
  EXPECT_EQ(b.to_vector(), std::vector<int>({0}));
  b |= 1;
  b |= 3;
  b |= 200;
  b |= 590;
  b |= 699;
  EXPECT_EQ(b.to_vector(), std::vector<int>({0, 1, 3, 200, 590, 699}));
  b ^= 590;
  b ^= 3;
  EXPECT_EQ(b.to_vector(), std::vector<int>({0, 1, 200, 699}));
  EXPECT_TRUE(b[0]);
  EXPECT_TRUE(b[1]);
  EXPECT_TRUE(b[200]);
  EXPECT_TRUE(b[699]);
  EXPECT_FALSE(b[698]);

  Bitmap b2(700);
  b2 |= 1;
  b2 |= 3;
  b2 |= 200;
  b2 |= 201;
  EXPECT_EQ((b & b2).count(), 2);

  // construct with 1-bits
  Bitmap b3(127, {0, 1, 2, 5, 125}), b4(127, {5, 125, 126});
  EXPECT_EQ(b3, Bitmap(127, {125, 5, 2, 2, 2, 1, 0}));
  EXPECT_EQ(b4, Bitmap(127, {125, 5, 126, 5}));

  EXPECT_EQ(b3 | 63, Bitmap(127, {0, 1, 2, 5, 125, 63}));
  EXPECT_EQ(b3 | 125, Bitmap(127, {0, 1, 2, 5, 125}));
  EXPECT_EQ(b3 & 63, Bitmap(127));
  EXPECT_EQ(b3 & 125, Bitmap(127, 125));
  EXPECT_EQ(b3 ^ 63, Bitmap(127, {0, 1, 2, 5, 125, 63}));
  EXPECT_EQ(b3 ^ 125, Bitmap(127, {0, 1, 2, 5}));
  EXPECT_EQ(b3 - 63, Bitmap(127, {0, 1, 2, 5, 125}));
  EXPECT_EQ(b3 - 125, Bitmap(127, {0, 1, 2, 5}));

  EXPECT_EQ(b3 | b4, Bitmap(127, {0, 1, 2, 5, 125, 126}));
  EXPECT_EQ(b3 & b4, Bitmap(127, {5, 125}));
  EXPECT_EQ(b3 ^ b4, Bitmap(127, {0, 1, 2, 126}));
  EXPECT_EQ(b3 - b4, Bitmap(127, {0, 1, 2}));

  EXPECT_TRUE((b3).subset(b3));
  EXPECT_TRUE((b3 - b4).subset(b3));
  EXPECT_FALSE((b3).subset(b3 - b4));

  EXPECT_TRUE((b3).superset(b3));
  EXPECT_FALSE((b3 - b4).superset(b3));
  EXPECT_TRUE((b3).superset(b3 - b4));

  Bitmap bm0(0);
  Bitmap bm1(1);
  bm1 = ~bm1;
  Bitmap bm2(2);
  bm2 = ~bm2;
  Bitmap bm16(16);
  bm16 = ~bm16;
  Bitmap bm32(32);
  bm32 = ~bm32;
  Bitmap bm63(63);
  bm63 = ~bm63;
  Bitmap bm64(64);
  bm64 = ~bm64;

  EXPECT_EQ(bm0.count(), 0);
  EXPECT_EQ(bm1.count(), 1);
  EXPECT_EQ(bm2.count(), 2);
  EXPECT_EQ(bm16.count(), 16);
  EXPECT_EQ(bm32.count(), 32);
  EXPECT_EQ(bm63.count(), 63);
  EXPECT_EQ(bm64.count(), 64);

  Bitmap bb(128, {50, 90, 127});
  EXPECT_EQ(bb.front(), 50);
  EXPECT_EQ(bb.pop_front(), 50);
  EXPECT_EQ(bb.front(), 90);
  EXPECT_EQ(bb.pop_front(), 90);
  EXPECT_EQ(bb.front(), 127);
  EXPECT_EQ(bb.pop_front(), 127);
  EXPECT_EQ(bb.front(), -1);
  EXPECT_EQ(bb.pop_front(), -1);
}

TEST(BitmapTest, EncodeTest) {
  Bitmap b0(128), b1(128, 0);
  EXPECT_EQ(b0.to_string(), "00000000000000000000000000000000");
  EXPECT_EQ(b1.to_string(), "00000000000000000000000000000001");
  EXPECT_EQ((~b0).to_string(), "ffffffffffffffffffffffffffffffff");
  EXPECT_EQ((~b1).to_string(), "fffffffffffffffffffffffffffffffe");
}
