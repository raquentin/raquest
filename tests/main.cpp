#include <gtest/gtest.h>

TEST(DummyTest, Addition) {
    EXPECT_EQ(1 + 1, 2);
}

TEST(DummyTest, Subtraction) {
    EXPECT_EQ(5 - 3, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
