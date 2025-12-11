#pragma once

#include "trlc/macro/concat.hpp"
#include "trlc/macro/count.hpp"
#include "trlc/macro/foreach_expr.hpp"
#include "trlc/macro/foreach_expr_indexed.hpp"
#include "trlc/macro/foreach_stmt.hpp"
#include "trlc/macro/foreach_stmt_indexed.hpp"

namespace trlc {

/**
 * @brief Count the number of variadic arguments
 *
 * @param ... Variable number of arguments
 * @return The count of arguments as a compile-time constant
 *
 * @example
 * TRLC_COUNT(a, b, c) → 3
 * TRLC_COUNT(x) → 1
 */
#define TRLC_COUNT(...) _TRLC_COUNT(__VA_ARGS__)

/**
 * @brief Apply a macro to each argument with semicolon separation
 *
 * @param MACRO The macro to apply to each argument
 * @param ... Variable number of arguments
 *
 * @example
 * #define PRINT(x) std::cout << #x << std::endl
 * TRLC_FOREACH_STMT(PRINT, a, b, c)
 * // Expands to: PRINT(a); PRINT(b); PRINT(c);
 *
 * Use case: Generating statements for each argument
 */
#define TRLC_FOREACH_STMT(MACRO, ...) _TRLC_FOREACH_STMT(MACRO, __VA_ARGS__)

/**
 * @brief Apply a macro to each argument with its 0-based index
 *
 * @param MACRO The macro to apply (must accept 2 parameters: value and index)
 * @param ... Variable number of arguments
 *
 * @example
 * #define PRINT_INDEXED(x, i) std::cout << i << ": " << #x << std::endl
 * TRLC_FOREACH_STMT_INDEXED(PRINT_INDEXED, a, b, c)
 * // Expands to: PRINT_INDEXED(a, 0); PRINT_INDEXED(b, 1); PRINT_INDEXED(c, 2);
 *
 * Use case: When you need to know the position of each argument (0-based)
 */
#define TRLC_FOREACH_STMT_INDEXED(MACRO, ...) _TRLC_FOREACH_STMT_INDEXED(MACRO, __VA_ARGS__)

/**
 * @brief Expand a macro for each argument with comma separation
 *
 * @param MACRO The macro to expand for each argument
 * @param ... Variable number of arguments
 *
 * @example
 * #define TYPE(x) decltype(x)
 * TRLC_FOREACH_EXPR(TYPE, a, b, c)
 * // Expands to: TYPE(a), TYPE(b), TYPE(c)
 *
 * Use case: Generating comma-separated lists (arrays, function arguments, template parameters)
 */
#define TRLC_FOREACH_EXPR(MACRO, ...) _TRLC_FOREACH_EXPR(MACRO, __VA_ARGS__)

/**
 * @brief Expand a macro for each argument with index, comma-separated
 *
 * @param MACRO The macro to expand (must accept 2 parameters: value and index)
 * @param ... Variable number of arguments
 *
 * @example
 * #define PAIR(x, i) std::pair<decltype(x), int>{x, i}
 * TRLC_FOREACH_EXPR_INDEXED(PAIR, a, b, c)
 * // Expands to: PAIR(a, 0), PAIR(b, 1), PAIR(c, 2)
 *
 * Use case: Building indexed data structures, enumeration with positions (0-based)
 */
#define TRLC_FOREACH_EXPR_INDEXED(MACRO, ...) _TRLC_FOREACH_EXPR_INDEXED(MACRO, __VA_ARGS__)

/**
 * @brief Concatenate all arguments with delimiter then apply macro
 *
 * @param MACRO The macro to apply to the concatenated result
 * @param delim The delimiter to use for concatenation (e.g., _ or -)
 * @param ... Variable number of arguments
 *
 * @example
 * #define MAKE_ID(x) constexpr auto x = #x
 * TRLC_CONCAT(MAKE_ID, _, foo, bar, baz)
 * // Expands to: MAKE_ID(foo_bar_baz)
 * // Result: constexpr auto foo_bar_baz = "foo_bar_baz"
 *
 * Use case: Creating compound identifiers or concatenated names
 */
#define TRLC_CONCAT(MACRO, delim, ...) _TRLC_CONCAT(MACRO, delim, __VA_ARGS__)

/**
 * @brief Convert argument to string literal
 *
 * @param x The argument to stringify
 * @return String literal of the argument
 *
 * @example
 * TRLC_STRINGIFY(Hello) → "Hello"
 * TRLC_STRINGIFY(123) → "123"
 */
#define TRLC_STRINGIFY(x) #x

}  // namespace trlc
