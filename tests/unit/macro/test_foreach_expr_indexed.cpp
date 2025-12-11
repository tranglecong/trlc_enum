/**
 * @file test_foreach_expr_indexed.cpp
 * @brief Unit tests for TRLC_FOREACH_EXPR_INDEXED macro
 */

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "trlc/macro.hpp"

#include <gtest/gtest.h>

namespace trlc::test {

/**
 * @brief Helper macro to create pair
 */
#define MAKE_PAIR(val, idx) \
    std::pair<int, int> {   \
        val, idx            \
    }

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with pair creation
 */
TEST(TrlcForeachExprIndexedTest, PairCreation) {
    std::vector<std::pair<int, int>> pairs = {TRLC_FOREACH_EXPR_INDEXED(MAKE_PAIR, 10, 20, 30)};

    ASSERT_EQ(pairs.size(), 3);
    EXPECT_EQ(pairs[0].first, 10);
    EXPECT_EQ(pairs[0].second, 0);
    EXPECT_EQ(pairs[1].first, 20);
    EXPECT_EQ(pairs[1].second, 1);
    EXPECT_EQ(pairs[2].first, 30);
    EXPECT_EQ(pairs[2].second, 2);
}

#undef MAKE_PAIR

/**
 * @brief Helper macro for value plus index
 */
#define VAL_PLUS_IDX(val, idx) ((val) + (idx))

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with arithmetic
 */
TEST(TrlcForeachExprIndexedTest, ArithmeticWithIndex) {
    constexpr int results[] = {TRLC_FOREACH_EXPR_INDEXED(VAL_PLUS_IDX, 100, 200, 300, 400)};

    EXPECT_EQ(results[0], 100);  // 100 + 0
    EXPECT_EQ(results[1], 201);  // 200 + 1
    EXPECT_EQ(results[2], 302);  // 300 + 2
    EXPECT_EQ(results[3], 403);  // 400 + 3
}

#undef VAL_PLUS_IDX

/**
 * @brief Helper macro for multiplying value by index
 */
#define MULTIPLY_BY_INDEX(val, idx) ((val) * (idx))

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with multiplication
 */
TEST(TrlcForeachExprIndexedTest, Multiplication) {
    constexpr int results[] = {TRLC_FOREACH_EXPR_INDEXED(MULTIPLY_BY_INDEX, 5, 10, 15, 20, 25)};

    EXPECT_EQ(results[0], 0);    // 5 * 0
    EXPECT_EQ(results[1], 10);   // 10 * 1
    EXPECT_EQ(results[2], 30);   // 15 * 2
    EXPECT_EQ(results[3], 60);   // 20 * 3
    EXPECT_EQ(results[4], 100);  // 25 * 4
}

#undef MULTIPLY_BY_INDEX

/**
 * @brief Helper struct for testing
 */
struct IndexedValue {
    int value;
    int index;

    constexpr bool operator==(const IndexedValue& other) const {
        return value == other.value && index == other.index;
    }
};

#define CREATE_INDEXED_VALUE(v, i) \
    IndexedValue {                 \
        v, i                       \
    }

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with custom struct
 */
TEST(TrlcForeachExprIndexedTest, CustomStruct) {
    constexpr std::array<IndexedValue, 4> arr = {
        TRLC_FOREACH_EXPR_INDEXED(CREATE_INDEXED_VALUE, 50, 60, 70, 80)};

    EXPECT_EQ(arr[0], (IndexedValue{50, 0}));
    EXPECT_EQ(arr[1], (IndexedValue{60, 1}));
    EXPECT_EQ(arr[2], (IndexedValue{70, 2}));
    EXPECT_EQ(arr[3], (IndexedValue{80, 3}));
}

#undef CREATE_INDEXED_VALUE

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with single argument
 */
TEST(TrlcForeachExprIndexedTest, SingleArgument) {
#define PAIR(v, i) std::pair<int, int>{v, i}

    std::vector<std::pair<int, int>> vec = {TRLC_FOREACH_EXPR_INDEXED(PAIR, 999)};

#undef PAIR

    ASSERT_EQ(vec.size(), 1);
    EXPECT_EQ(vec[0].first, 999);
    EXPECT_EQ(vec[0].second, 0);
}

/**
 * @brief Helper macro for string formatting
 */
#define FORMAT_STR_IDX(s, i) std::string(#s) + "[" + std::to_string(i) + "]"

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with string formatting
 */
TEST(TrlcForeachExprIndexedTest, StringFormatting) {
    std::vector<std::string> strings = {
        TRLC_FOREACH_EXPR_INDEXED(FORMAT_STR_IDX, item, element, data)};

    ASSERT_EQ(strings.size(), 3);
    EXPECT_EQ(strings[0], "item[0]");
    EXPECT_EQ(strings[1], "element[1]");
    EXPECT_EQ(strings[2], "data[2]");
}

#undef FORMAT_STR_IDX

/**
 * @brief Helper macro for conditional value
 */
#define CONDITIONAL_VALUE(v, i) ((i) % 2 == 0 ? (v) : -(v))

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with conditional expression
 */
TEST(TrlcForeachExprIndexedTest, ConditionalExpression) {
    constexpr int results[] = {TRLC_FOREACH_EXPR_INDEXED(CONDITIONAL_VALUE, 1, 2, 3, 4, 5, 6)};

    EXPECT_EQ(results[0], 1);   // index 0 (even): +1
    EXPECT_EQ(results[1], -2);  // index 1 (odd): -2
    EXPECT_EQ(results[2], 3);   // index 2 (even): +3
    EXPECT_EQ(results[3], -4);  // index 3 (odd): -4
    EXPECT_EQ(results[4], 5);   // index 4 (even): +5
    EXPECT_EQ(results[5], -6);  // index 5 (odd): -6
}

#undef CONDITIONAL_VALUE

/**
 * @brief Helper for power calculation
 */
#define POWER(base, exp)                         \
    [](int b, int e) {                           \
        int result = 1;                          \
        for (int i = 0; i < e; ++i) result *= b; \
        return result;                           \
    }(base, exp)

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with power calculation
 */
TEST(TrlcForeachExprIndexedTest, PowerCalculation) {
    const int powers[] = {TRLC_FOREACH_EXPR_INDEXED(POWER, 2, 3, 4, 5)};

    EXPECT_EQ(powers[0], 1);    // 2^0 = 1
    EXPECT_EQ(powers[1], 3);    // 3^1 = 3
    EXPECT_EQ(powers[2], 16);   // 4^2 = 16
    EXPECT_EQ(powers[3], 125);  // 5^3 = 125
}

#undef POWER

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with array of pairs
 */
TEST(TrlcForeachExprIndexedTest, ArrayOfPairs) {
#define P(v, i) std::make_pair(v, i)

    constexpr std::pair<int, int> pairs[] = {TRLC_FOREACH_EXPR_INDEXED(P, 7, 14, 21, 28, 35, 42)};

#undef P

    for (int i = 0; i < 6; ++i) {
        EXPECT_EQ(pairs[i].first, (i + 1) * 7);
        EXPECT_EQ(pairs[i].second, i);
    }
}

/**
 * @brief Test TRLC_FOREACH_EXPR_INDEXED with many arguments (15)
 */
TEST(TrlcForeachExprIndexedTest, FifteenArguments) {
#define SUM(v, i) ((v) + (i))

    constexpr int results[] = {TRLC_FOREACH_EXPR_INDEXED(
        SUM, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150)};

#undef SUM

    ASSERT_EQ(sizeof(results) / sizeof(int), 15);
    for (int i = 0; i < 15; ++i) {
        EXPECT_EQ(results[i], (i + 1) * 10 + i);
    }
}

}  // namespace trlc::test
