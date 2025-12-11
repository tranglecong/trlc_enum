/**
 * @file test_foreach_stmt.cpp
 * @brief Unit tests for TRLC_FOREACH_STMT macro
 */

#include <sstream>
#include <string>

#include "trlc/macro.hpp"

#include <gtest/gtest.h>

namespace trlc::test {

/**
 * @brief Helper macro to append values to a string stream
 */
#define APPEND_TO_STREAM(x) ss << #x << ";"

/**
 * @brief Test TRLC_FOREACH_STMT with single argument
 */
TEST(TrlcForeachStmtTest, SingleArgument) {
    std::stringstream ss;
    TRLC_FOREACH_STMT(APPEND_TO_STREAM, apple);
    EXPECT_EQ(ss.str(), "apple;");
}

/**
 * @brief Test TRLC_FOREACH_STMT with multiple arguments
 */
TEST(TrlcForeachStmtTest, MultipleArguments) {
    std::stringstream ss;
    TRLC_FOREACH_STMT(APPEND_TO_STREAM, apple, banana, cherry);
    EXPECT_EQ(ss.str(), "apple;banana;cherry;");
}

/**
 * @brief Test TRLC_FOREACH_STMT with five arguments
 */
TEST(TrlcForeachStmtTest, FiveArguments) {
    std::stringstream ss;
    TRLC_FOREACH_STMT(APPEND_TO_STREAM, one, two, three, four, five);
    EXPECT_EQ(ss.str(), "one;two;three;four;five;");
}

#undef APPEND_TO_STREAM

/**
 * @brief Helper macro for counter test
 */
#define INCREMENT_COUNTER(x) counter++

/**
 * @brief Test TRLC_FOREACH_STMT generates correct number of statements
 */
TEST(TrlcForeachStmtTest, CountStatements) {
    int counter = 0;
    TRLC_FOREACH_STMT(INCREMENT_COUNTER, a, b, c, d, e, f, g, h, i, j);
    EXPECT_EQ(counter, 10);
}

#undef INCREMENT_COUNTER

/**
 * @brief Helper macro for vector test
 */
#define PUSH_TO_VECTOR(x) vec.push_back(x)

/**
 * @brief Test TRLC_FOREACH_STMT with actual values
 */
TEST(TrlcForeachStmtTest, PushToVector) {
    std::vector<int> vec;
    TRLC_FOREACH_STMT(PUSH_TO_VECTOR, 10, 20, 30, 40, 50);

    ASSERT_EQ(vec.size(), 5);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_EQ(vec[2], 30);
    EXPECT_EQ(vec[3], 40);
    EXPECT_EQ(vec[4], 50);
}

#undef PUSH_TO_VECTOR

/**
 * @brief Helper struct for testing
 */
struct TestStruct {
    static inline int call_count = 0;

    static void reset() { call_count = 0; }

    static void increment() { call_count++; }
};

#define CALL_INCREMENT(x) TestStruct::increment()

/**
 * @brief Test TRLC_FOREACH_STMT with static method calls
 */
TEST(TrlcForeachStmtTest, StaticMethodCalls) {
    TestStruct::reset();
    TRLC_FOREACH_STMT(CALL_INCREMENT, a, b, c, d, e);
    EXPECT_EQ(TestStruct::call_count, 5);
}

#undef CALL_INCREMENT

/**
 * @brief Test TRLC_FOREACH_STMT with many arguments (20)
 */
TEST(TrlcForeachStmtTest, TwentyArguments) {
    int counter = 0;
#define COUNT_IT(x) counter++
    TRLC_FOREACH_STMT(
        COUNT_IT, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
#undef COUNT_IT
    EXPECT_EQ(counter, 20);
}

}  // namespace trlc::test
