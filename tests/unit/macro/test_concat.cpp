/**
 * @file test_concat.cpp
 * @brief Unit tests for TRLC_CONCAT macro
 */

#include <cstring>
#include <string>

#include "trlc/macro.hpp"

#include <gtest/gtest.h>

namespace trlc::test {

/**
 * @brief Helper macro for creating identifiers
 */
#define MAKE_IDENTIFIER(x) x

/**
 * @brief Test TRLC_CONCAT with two arguments
 */
TEST(TrlcConcatTest, TwoArguments) {
#define CREATE_VAR(name) int name = 42
    TRLC_CONCAT(CREATE_VAR, _, hello, world);
#undef CREATE_VAR

    EXPECT_EQ(hello_world, 42);
}

/**
 * @brief Test TRLC_CONCAT with three arguments
 */
TEST(TrlcConcatTest, ThreeArguments) {
#define DECLARE_INT(name) int name = 100
    TRLC_CONCAT(DECLARE_INT, _, my, variable, name);
#undef DECLARE_INT

    EXPECT_EQ(my_variable_name, 100);
}

/**
 * @brief Test TRLC_CONCAT with underscore delimiter
 */
TEST(TrlcConcatTest, UnderscoreDelimiter) {
#define DEF_CONST(id) constexpr int id = 999
    TRLC_CONCAT(DEF_CONST, _, prefix, test, suffix);
#undef DEF_CONST

    EXPECT_EQ(prefix_test_suffix, 999);
}

/**
 * @brief Test TRLC_CONCAT with empty delimiter
 */
TEST(TrlcConcatTest, NoDelimiter) {
#define CREATE_NAME(n) constexpr const char* n = #n
    TRLC_CONCAT(CREATE_NAME, , foo, bar, baz);
#undef CREATE_NAME

    EXPECT_STREQ(foobarbaz, "foobarbaz");
}

/**
 * @brief Test TRLC_CONCAT with single argument
 */
TEST(TrlcConcatTest, SingleArgument) {
#define MAKE_VAR(v) int v = 55
    TRLC_CONCAT(MAKE_VAR, _, single);
#undef MAKE_VAR

    EXPECT_EQ(single, 55);
}

/**
 * @brief Test TRLC_CONCAT for creating function names
 */
TEST(TrlcConcatTest, FunctionName) {
#define DEFINE_FUNC(name) auto name = []() -> int { return 123; }

    TRLC_CONCAT(DEFINE_FUNC, _, get, value, func);
#undef DEFINE_FUNC

    EXPECT_EQ(get_value_func(), 123);
}

/**
 * @brief Test TRLC_CONCAT with many arguments
 */
TEST(TrlcConcatTest, FiveArguments) {
#define VAR_INIT(id) double id = 3.14
    TRLC_CONCAT(VAR_INIT, _, a, b, c, d, e);
#undef VAR_INIT

    EXPECT_DOUBLE_EQ(a_b_c_d_e, 3.14);
}

/**
 * @brief Test TRLC_CONCAT with X delimiter
 */
TEST(TrlcConcatTest, CustomDelimiter) {
#define STR(s) #s
    const char* concat_result = TRLC_CONCAT(STR, X, one, two, three);
#undef STR

    EXPECT_STREQ(concat_result, "oneXtwoXthree");
}

/**
 * @brief Helper struct for namespace simulation
 */
struct TestNamespace {
    static inline int var_a_b_c = 0;
};

/**
 * @brief Test TRLC_CONCAT with struct member
 */
TEST(TrlcConcatTest, StructMember) {
#define ASSIGN_MEMBER(name) TestNamespace::name = 777
    TRLC_CONCAT(ASSIGN_MEMBER, _, var, a, b, c);
#undef ASSIGN_MEMBER

    EXPECT_EQ(TestNamespace::var_a_b_c, 777);
}

/**
 * @brief Test TRLC_CONCAT with enum creation
 */
TEST(TrlcConcatTest, EnumValue) {
#define CREATE_ENUM_VAR(name)                   \
    enum class name { Value1, Value2, Value3 }; \
    name instance = name::Value2

    TRLC_CONCAT(CREATE_ENUM_VAR, _, My, Enum, Type);
#undef CREATE_ENUM_VAR

    EXPECT_EQ(instance, My_Enum_Type::Value2);
}

/**
 * @brief Test TRLC_CONCAT with string literal concatenation
 */
TEST(TrlcConcatTest, StringLiteral) {
#define TO_STRING(x) std::string(#x)
    std::string result = TRLC_CONCAT(TO_STRING, _, hello, world, test);
#undef TO_STRING

    EXPECT_EQ(result, "hello_world_test");
}

/**
 * @brief Test TRLC_CONCAT with array name generation
 */
TEST(TrlcConcatTest, ArrayGeneration) {
#define DEFINE_ARRAY(name) int name[] = {1, 2, 3, 4, 5}
    TRLC_CONCAT(DEFINE_ARRAY, _, my, test, array);
#undef DEFINE_ARRAY

    EXPECT_EQ(my_test_array[0], 1);
    EXPECT_EQ(my_test_array[4], 5);
    EXPECT_EQ(sizeof(my_test_array) / sizeof(int), 5);
}

/**
 * @brief Test TRLC_CONCAT with ten arguments
 */
TEST(TrlcConcatTest, TenArguments) {
#define MAKE_LONG_NAME(n) const char* n = #n
    TRLC_CONCAT(MAKE_LONG_NAME, _, a, b, c, d, e, f, g, h, i, j);
#undef MAKE_LONG_NAME

    EXPECT_STREQ(a_b_c_d_e_f_g_h_i_j, "a_b_c_d_e_f_g_h_i_j");
}

}  // namespace trlc::test
