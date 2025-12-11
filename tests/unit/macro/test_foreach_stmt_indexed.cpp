/**
 * @file test_foreach_stmt_indexed.cpp
 * @brief Unit tests for TRLC_FOREACH_STMT_INDEXED macro
 */

#include <sstream>
#include <string>
#include <vector>

#include "trlc/macro.hpp"

#include <gtest/gtest.h>

namespace trlc::test {

/**
 * @brief Helper macro to append value with index to string stream
 */
#define APPEND_WITH_INDEX(val, idx) ss << #val << ":" << idx << ";"

/**
 * @brief Test TRLC_FOREACH_STMT_INDEXED with single argument
 */
TEST(TrlcForeachStmtIndexedTest, SingleArgument) {
    std::stringstream ss;
    TRLC_FOREACH_STMT_INDEXED(APPEND_WITH_INDEX, apple);
    EXPECT_EQ(ss.str(), "apple:0;");
}

/**
 * @brief Test TRLC_FOREACH_STMT_INDEXED with multiple arguments
 */
TEST(TrlcForeachStmtIndexedTest, MultipleArguments) {
    std::stringstream ss;
    TRLC_FOREACH_STMT_INDEXED(APPEND_WITH_INDEX, first, second, third);
    EXPECT_EQ(ss.str(), "first:0;second:1;third:2;");
}

/**
 * @brief Test TRLC_FOREACH_STMT_INDEXED with five arguments
 */
TEST(TrlcForeachStmtIndexedTest, FiveArguments) {
    std::stringstream ss;
    TRLC_FOREACH_STMT_INDEXED(APPEND_WITH_INDEX, a, b, c, d, e);
    EXPECT_EQ(ss.str(), "a:0;b:1;c:2;d:3;e:4;");
}

#undef APPEND_WITH_INDEX

/**
 * @brief Helper macro for index verification
 */
#define VERIFY_INDEX(val, idx) EXPECT_EQ(idx, expected_indices[idx])

/**
 * @brief Test TRLC_FOREACH_STMT_INDEXED generates correct indices
 */
TEST(TrlcForeachStmtIndexedTest, VerifyIndices) {
    std::vector<int> expected_indices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    TRLC_FOREACH_STMT_INDEXED(VERIFY_INDEX, a, b, c, d, e, f, g, h, i, j);
}

#undef VERIFY_INDEX

/**
 * @brief Helper struct for testing indexed operations
 */
struct IndexedData {
    int value;
    int index;
};

#define CREATE_INDEXED_DATA(val, idx) indexed_vec.push_back(IndexedData{val, idx})

/**
 * @brief Test TRLC_FOREACH_STMT_INDEXED with structured data
 */
TEST(TrlcForeachStmtIndexedTest, StructuredData) {
    std::vector<IndexedData> indexed_vec;
    TRLC_FOREACH_STMT_INDEXED(CREATE_INDEXED_DATA, 100, 200, 300, 400);

    ASSERT_EQ(indexed_vec.size(), 4);
    for (size_t i = 0; i < indexed_vec.size(); ++i) {
        EXPECT_EQ(indexed_vec[i].index, static_cast<int>(i));
        EXPECT_EQ(indexed_vec[i].value, (i + 1) * 100);
    }
}

#undef CREATE_INDEXED_DATA

/**
 * @brief Test TRLC_FOREACH_STMT_INDEXED with sum accumulation
 */
TEST(TrlcForeachStmtIndexedTest, SumWithIndices) {
    int sum = 0;
#define ADD_VALUE_AND_INDEX(val, idx) sum += (val + idx)

    TRLC_FOREACH_STMT_INDEXED(ADD_VALUE_AND_INDEX, 10, 20, 30);
    // sum = (10+0) + (20+1) + (30+2) = 10 + 21 + 32 = 63
    EXPECT_EQ(sum, 63);

#undef ADD_VALUE_AND_INDEX
}

/**
 * @brief Test TRLC_FOREACH_STMT_INDEXED with array initialization
 */
TEST(TrlcForeachStmtIndexedTest, ArrayIndexVerification) {
    std::vector<std::pair<int, int>> pairs;
#define PUSH_PAIR(val, idx) pairs.push_back({val, idx})

    TRLC_FOREACH_STMT_INDEXED(PUSH_PAIR, 5, 10, 15, 20, 25);

#undef PUSH_PAIR

    ASSERT_EQ(pairs.size(), 5);
    EXPECT_EQ(pairs[0].first, 5);
    EXPECT_EQ(pairs[0].second, 0);
    EXPECT_EQ(pairs[1].first, 10);
    EXPECT_EQ(pairs[1].second, 1);
    EXPECT_EQ(pairs[2].first, 15);
    EXPECT_EQ(pairs[2].second, 2);
    EXPECT_EQ(pairs[3].first, 20);
    EXPECT_EQ(pairs[3].second, 3);
    EXPECT_EQ(pairs[4].first, 25);
    EXPECT_EQ(pairs[4].second, 4);
}

/**
 * @brief Test TRLC_FOREACH_STMT_INDEXED with string concatenation
 */
TEST(TrlcForeachStmtIndexedTest, StringConcat) {
    std::string result;
#define CONCAT_STR_INDEX(s, i) result += std::string(#s) + std::to_string(i)

    TRLC_FOREACH_STMT_INDEXED(CONCAT_STR_INDEX, x, y, z);
    EXPECT_EQ(result, "x0y1z2");

#undef CONCAT_STR_INDEX
}

/**
 * @brief Test TRLC_FOREACH_STMT_INDEXED with many arguments (15)
 */
TEST(TrlcForeachStmtIndexedTest, FifteenArguments) {
    int index_sum = 0;
#define SUM_INDEX(val, idx) index_sum += idx

    TRLC_FOREACH_STMT_INDEXED(SUM_INDEX, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o);

#undef SUM_INDEX

    // Sum of indices 0 to 14 = 14 * 15 / 2 = 105
    EXPECT_EQ(index_sum, 105);
}

}  // namespace trlc::test
