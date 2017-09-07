#include <gtest/gtest.h>

#include "hex.h"

TEST(HexDistance, One)
{
    EXPECT_EQ(1, hex::distance(hex::Pos(1, 1), hex::Pos(0, 1)));
    EXPECT_EQ(1, hex::distance(hex::Pos(1, 1), hex::Pos(2, 1)));
    EXPECT_EQ(1, hex::distance(hex::Pos(1, 1), hex::Pos(1, 0)));
    EXPECT_EQ(1, hex::distance(hex::Pos(1, 1), hex::Pos(2, 0)));
    EXPECT_EQ(1, hex::distance(hex::Pos(1, 1), hex::Pos(1, 2)));
    EXPECT_EQ(1, hex::distance(hex::Pos(1, 1), hex::Pos(2, 2)));
}
