/**
 * @file test_count.cpp
 * @brief Unit tests for TRLC_COUNT macro
 */

#include "trlc/config.hpp"
#include "trlc/macro.hpp"

#include <gtest/gtest.h>

namespace trlc::test {

/**
 * @brief Test TRLC_COUNT with single argument
 */
TEST(TrlcCountTest, SingleArgument) {
    constexpr int count = TRLC_COUNT(a);
    EXPECT_EQ(count, 1);
}

/**
 * @brief Test TRLC_COUNT with two arguments
 */
TEST(TrlcCountTest, TwoArguments) {
    constexpr int count = TRLC_COUNT(a, b);
    EXPECT_EQ(count, 2);
}

/**
 * @brief Test TRLC_COUNT with three arguments
 */
TEST(TrlcCountTest, ThreeArguments) {
    constexpr int count = TRLC_COUNT(a, b, c);
    EXPECT_EQ(count, 3);
}

/**
 * @brief Test TRLC_COUNT with multiple arguments (10)
 */
TEST(TrlcCountTest, TenArguments) {
    constexpr int count = TRLC_COUNT(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    EXPECT_EQ(count, 10);
}

/**
 * @brief Test TRLC_COUNT with many arguments (20)
 */
TEST(TrlcCountTest, TwentyArguments) {
    constexpr int count =
        TRLC_COUNT(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
    EXPECT_EQ(count, 20);
}

/**
 * @brief Test TRLC_COUNT with different types of arguments
 */
TEST(TrlcCountTest, MixedTypes) {
    constexpr int count = TRLC_COUNT(1, "string", 3.14, true, 'c');
    EXPECT_EQ(count, 5);
}

/**
 * @brief Test TRLC_COUNT with complex expressions
 */
TEST(TrlcCountTest, ComplexExpressions) {
    constexpr int count = TRLC_COUNT(1 + 1, 2 * 3, 4 - 1);
    EXPECT_EQ(count, 3);
}

}  // namespace trlc::test
