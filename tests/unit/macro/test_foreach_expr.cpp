/**
 * @file test_foreach_expr.cpp
 * @brief Unit tests for TRLC_FOREACH_EXPR macro
 */

#include <array>
#include <string>
#include <type_traits>
#include <vector>

#include "trlc/macro.hpp"

#include <gtest/gtest.h>

namespace trlc::test {

/**
 * @brief Helper macro for type conversion
 */
#define TO_INT(x) static_cast<int>(x)

/**
 * @brief Test TRLC_FOREACH_EXPR with array initialization
 */
TEST(TrlcForeachExprTest, ArrayInitialization) {
    int arr[] = {TRLC_FOREACH_EXPR(TO_INT, 1, 2, 3, 4, 5)};

    ASSERT_EQ(sizeof(arr) / sizeof(int), 5);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[3], 4);
    EXPECT_EQ(arr[4], 5);
}

#undef TO_INT

/**
 * @brief Helper macro for creating string literals
 */
#define MAKE_STRING(x) std::string(#x)

/**
 * @brief Test TRLC_FOREACH_EXPR with vector initialization
 */
TEST(TrlcForeachExprTest, VectorInitialization) {
    std::vector<std::string> vec = {TRLC_FOREACH_EXPR(MAKE_STRING, apple, banana, cherry)};

    ASSERT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], "apple");
    EXPECT_EQ(vec[1], "banana");
    EXPECT_EQ(vec[2], "cherry");
}

#undef MAKE_STRING

/**
 * @brief Helper macro for doubling values
 */
#define DOUBLE(x) ((x) * 2)

/**
 * @brief Test TRLC_FOREACH_EXPR with expressions
 */
TEST(TrlcForeachExprTest, ExpressionExpansion) {
    constexpr int arr[] = {TRLC_FOREACH_EXPR(DOUBLE, 1, 2, 3, 4)};

    EXPECT_EQ(arr[0], 2);
    EXPECT_EQ(arr[1], 4);
    EXPECT_EQ(arr[2], 6);
    EXPECT_EQ(arr[3], 8);
}

#undef DOUBLE

/**
 * @brief Test TRLC_FOREACH_EXPR with std::array
 */
TEST(TrlcForeachExprTest, StdArrayInit) {
#define AS_IS(x) x
    constexpr std::array<int, 7> arr = {TRLC_FOREACH_EXPR(AS_IS, 10, 20, 30, 40, 50, 60, 70)};
#undef AS_IS

    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[6], 70);
}

/**
 * @brief Helper macro for type conversion
 */
#define TO_DOUBLE(x) static_cast<double>(x)

/**
 * @brief Test TRLC_FOREACH_EXPR with floating point values
 */
TEST(TrlcForeachExprTest, FloatingPoint) {
    double values[] = {TRLC_FOREACH_EXPR(TO_DOUBLE, 1, 2, 3, 4, 5)};

    for (int i = 0; i < 5; ++i) {
        EXPECT_DOUBLE_EQ(values[i], static_cast<double>(i + 1));
    }
}

#undef TO_DOUBLE

/**
 * @brief Helper template for testing
 */
template <typename T>
struct Wrapper {
    T value;
};

#define WRAP_INT(x) \
    Wrapper<int> {  \
        x           \
    }

/**
 * @brief Test TRLC_FOREACH_EXPR with struct initialization
 */
TEST(TrlcForeachExprTest, StructWrapper) {
    std::vector<Wrapper<int>> wrappers = {TRLC_FOREACH_EXPR(WRAP_INT, 100, 200, 300)};

    ASSERT_EQ(wrappers.size(), 3);
    EXPECT_EQ(wrappers[0].value, 100);
    EXPECT_EQ(wrappers[1].value, 200);
    EXPECT_EQ(wrappers[2].value, 300);
}

#undef WRAP_INT

/**
 * @brief Test TRLC_FOREACH_EXPR with single argument
 */
TEST(TrlcForeachExprTest, SingleArgument) {
#define IDENTITY(x) x
    int arr[] = {TRLC_FOREACH_EXPR(IDENTITY, 42)};
#undef IDENTITY

    EXPECT_EQ(arr[0], 42);
}

/**
 * @brief Helper macro for string literal creation
 */
#define STR_LIT(x) #x

/**
 * @brief Test TRLC_FOREACH_EXPR with string literals
 */
TEST(TrlcForeachExprTest, StringLiterals) {
    const char* strings[] = {TRLC_FOREACH_EXPR(STR_LIT, red, green, blue, yellow)};

    EXPECT_STREQ(strings[0], "red");
    EXPECT_STREQ(strings[1], "green");
    EXPECT_STREQ(strings[2], "blue");
    EXPECT_STREQ(strings[3], "yellow");
}

#undef STR_LIT

/**
 * @brief Test TRLC_FOREACH_EXPR with complex macro
 */
TEST(TrlcForeachExprTest, ComplexMacro) {
#define SQUARE_PLUS_ONE(x) ((x) * (x) + 1)

    constexpr int results[] = {TRLC_FOREACH_EXPR(SQUARE_PLUS_ONE, 1, 2, 3, 4, 5)};

#undef SQUARE_PLUS_ONE

    EXPECT_EQ(results[0], 2);   // 1*1 + 1 = 2
    EXPECT_EQ(results[1], 5);   // 2*2 + 1 = 5
    EXPECT_EQ(results[2], 10);  // 3*3 + 1 = 10
    EXPECT_EQ(results[3], 17);  // 4*4 + 1 = 17
    EXPECT_EQ(results[4], 26);  // 5*5 + 1 = 26
}

/**
 * @brief Test TRLC_FOREACH_EXPR with many arguments (20)
 */
TEST(TrlcForeachExprTest, TwentyArguments) {
#define ID(x) x
    int arr[] = {TRLC_FOREACH_EXPR(
        ID, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)};
#undef ID

    ASSERT_EQ(sizeof(arr) / sizeof(int), 20);
    for (int i = 0; i < 20; ++i) {
        EXPECT_EQ(arr[i], i + 1);
    }
}

}  // namespace trlc::test
