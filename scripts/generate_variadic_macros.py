import sys

def generate_index_strings(n):
    """
    Generate individual string literal array for indices 0 to n-1
    Each index is stored as a separate null-terminated string
    
    No namespace - global scope with TRLC_ prefix to avoid naming conflicts
    """
    # Build array of individual string literals: {"0", "1", "2", ..., "N-1"}
    indices = [str(i) for i in range(n)]
    string_array = "{" + ", ".join([f'"{idx}"' for idx in indices]) + "}"
    
    index_strings_file = f"""
/**
 * @file [Generated File]
 * @brief Auto-generated compile-time index string literals
 * 
 * This file contains constexpr string_view array for indices 0 to {n-1}.
 * Global scope with TRLC_ prefix to avoid naming conflicts.
 * 
 * DO NOT MODIFY THIS FILE MANUALLY
 */
#pragma once

#include <string_view>
#include <array>

// clang-format off

// Global constexpr array with TRLC_ prefix to avoid naming conflicts
// Array of index string literals: {{"0", "1", "2", ..., "{n-1}"}}
constexpr std::array<std::string_view, {n}> TRLC_CONSTEXPR_INDEX_STRINGS = {string_array};

/**
 * @brief Get compile-time string_view for a specific index
 * @param idx The index to convert to string (0 to {n-1})
 * @return string_view for the index string, or empty string if out of range
 * 
 * This function is constexpr and can be used at compile-time.
 * 
 * Example:
 *   trlcConstexprIndexString(0) -> "0"
 *   trlcConstexprIndexString(42) -> "42"
 *   trlcConstexprIndexString(123) -> "123"
 * 
 * Implementation uses direct array lookup for O(1) access.
 */
constexpr std::string_view trlcConstexprIndexString(std::size_t idx) {{
    if (idx >= {n}) {{
        return "";  // Out of range
    }}
    return TRLC_CONSTEXPR_INDEX_STRINGS.at(idx);
}}

// clang-format on
"""
    return index_strings_file


def generate_macros(n):
    # Generate GET_ARG macros
    get_arg_macros = ""
    for i in range(1, n + 1):
        args = (
            ", ".join([f"arg{j}" for j in range(1, i + 1)]) + ", ..."
            if i > 1
            else "arg1, ..."
        )
        get_arg_macros += f"#define _TRLC_GET_ARG_{i}({args}) arg{i}\n"

    # Generate COUNT macros (with _ prefix for internal use)
    count_args_macro = f"#define _TRLC_COUNT(...) _TRLC_COUNT_IMPL(__VA_ARGS__, {', '.join(map(str, range(n, 0, -1)))})"
    count_args_impl_macro = f"#define _TRLC_COUNT_IMPL({', '.join([f'_{i}' for i in range(1, n + 1)])}, N, ...) N"

    # Generate recursive FOREACH_STMT_* macros (with _ prefix)
    apply_impl_macros = ""
    for i in range(1, n + 1):
        args = ", ".join([f"arg{j}" for j in range(1, i + 1)])
        apply_impl_macros += f"#define _TRLC_FOREACH_STMT_{i}(MACRO, {args}) "
        apply_impl_macros += "".join([f"MACRO(arg{j}); " for j in range(1, i + 1)])
        apply_impl_macros += "\n"

    # Generate FOREACH_STMT_INDEXED and recursive expansion macros with indexing (with _ prefix, 0-based index)
    apply_impl_macros_with_index = "\n".join(
        [
            f"#define _TRLC_FOREACH_STMT_INDEXED_{i}(MACRO, "
            + ", ".join([f"arg{j}" for j in range(1, i + 1)])
            + ") "
            + (
                "MACRO(arg1, 0); "
                + "".join([f"MACRO(arg{j}, {j-1}); " for j in range(2, i + 1)])
                if i > 1
                else "MACRO(arg1, 0);"
            )
            for i in range(1, n + 1)
        ]
    )

    # Generate recursive FOREACH_EXPR_* macros (with _ prefix)
    expand_impl_macros = ""
    for i in range(1, n + 1):
        args = ", ".join([f"arg{j}" for j in range(1, i + 1)])
        expand_impl_macros += f"#define _TRLC_FOREACH_EXPR_{i}(MACRO, {args}) "

        # Use a generator expression to join MACRO calls without a trailing comma
        expand_impl_macros += ", ".join([f"MACRO(arg{j})" for j in range(1, i + 1)])
        expand_impl_macros += "\n"

    # Generate FOREACH_EXPR_INDEXED and recursive expansion macros with indexing (with _ prefix, 0-based index)
    expand_impl_macros_with_index = "\n".join(
        [
            f"#define _TRLC_FOREACH_EXPR_INDEXED_{i}(MACRO, "
            + ", ".join([f"arg{j}" for j in range(1, i + 1)])
            + ") "
            + (
                "MACRO(arg1, 0)"
                + "".join([f", MACRO(arg{j}, {j-1})" for j in range(2, i + 1)])
                if i > 1
                else "MACRO(arg1, 0)"
            )
            for i in range(1, n + 1)
        ]
    )

    # Generate recursive CONCAT_* macros (with _ prefix)
    append_impl_macros = ""
    for i in range(1, n + 1):
        args = ",".join([f"arg{j}" for j in range(1, i + 1)])
        append_impl_macros += f"#define _TRLC_CONCAT_{i}(MACRO, delim, {args}) "

        # Use a generator expression to join MACRO calls without a trailing comma
        append_impl_macros += "MACRO(" + "##delim##".join([f"arg{j}" for j in range(1, i + 1)])
        append_impl_macros += ")\n"

    # Generate FOREACH_STMT macros (with _ prefix for internal implementation)
    apply_macros = "#define _TRLC_FOREACH_STMT(MACRO, ...) _TRLC_FOREACH_STMT_IMPL(MACRO, _TRLC_COUNT(__VA_ARGS__), __VA_ARGS__)\n"
    apply_macros += "#define _TRLC_FOREACH_STMT_IMPL(MACRO, COUNT, ...) _TRLC_FOREACH_STMT_IMPL2(MACRO, COUNT, __VA_ARGS__)\n"
    apply_macros += "#define _TRLC_FOREACH_STMT_IMPL2(MACRO, COUNT, ...) _TRLC_FOREACH_STMT_##COUNT(MACRO, __VA_ARGS__)"

    # Generate FOREACH_EXPR macros (with _ prefix for internal implementation)
    expand_macros = "#define _TRLC_FOREACH_EXPR(MACRO, ...) _TRLC_FOREACH_EXPR_IMPL(MACRO, _TRLC_COUNT(__VA_ARGS__), __VA_ARGS__)\n"
    expand_macros += "#define _TRLC_FOREACH_EXPR_IMPL(MACRO, COUNT, ...) _TRLC_FOREACH_EXPR_IMPL2(MACRO, COUNT, __VA_ARGS__)\n"
    expand_macros += "#define _TRLC_FOREACH_EXPR_IMPL2(MACRO, COUNT, ...) _TRLC_FOREACH_EXPR_##COUNT(MACRO, __VA_ARGS__)"

    # Generate CONCAT macros (with _ prefix for internal implementation)
    append_macros = "#define _TRLC_CONCAT(MACRO, delim,...) _TRLC_CONCAT_IMPL(MACRO, delim, _TRLC_COUNT(__VA_ARGS__), __VA_ARGS__)\n"
    append_macros += "#define _TRLC_CONCAT_IMPL(MACRO, delim, COUNT, ...) _TRLC_CONCAT_IMPL2(MACRO, delim, COUNT, __VA_ARGS__)\n"
    append_macros += "#define _TRLC_CONCAT_IMPL2(MACRO, delim, COUNT,...) _TRLC_CONCAT_##COUNT(MACRO, delim, __VA_ARGS__)"

    # Generate FOREACH_STMT_INDEXED macros (with _ prefix for internal implementation)
    apply_with_index_macros = "#define _TRLC_FOREACH_STMT_INDEXED(MACRO, ...) _TRLC_FOREACH_STMT_INDEXED_IMPL(MACRO, _TRLC_COUNT(__VA_ARGS__), __VA_ARGS__)\n"
    apply_with_index_macros += "#define _TRLC_FOREACH_STMT_INDEXED_IMPL(MACRO, COUNT, ...) _TRLC_FOREACH_STMT_INDEXED_IMPL2(MACRO, COUNT, __VA_ARGS__)\n"
    apply_with_index_macros += "#define _TRLC_FOREACH_STMT_INDEXED_IMPL2(MACRO, COUNT, ...) _TRLC_FOREACH_STMT_INDEXED_##COUNT(MACRO, __VA_ARGS__)"

    # Generate FOREACH_EXPR_INDEXED macros (with _ prefix for internal implementation)
    expand_with_index_macros = "#define _TRLC_FOREACH_EXPR_INDEXED(MACRO, ...) _TRLC_FOREACH_EXPR_INDEXED_IMPL(MACRO, _TRLC_COUNT(__VA_ARGS__), __VA_ARGS__)\n"
    expand_with_index_macros += "#define _TRLC_FOREACH_EXPR_INDEXED_IMPL(MACRO, COUNT, ...) _TRLC_FOREACH_EXPR_INDEXED_IMPL2(MACRO, COUNT, __VA_ARGS__)\n"
    expand_with_index_macros += "#define _TRLC_FOREACH_EXPR_INDEXED_IMPL2(MACRO, COUNT, ...) _TRLC_FOREACH_EXPR_INDEXED_##COUNT(MACRO, __VA_ARGS__)"

    header = """
/**
 * @file [Generated File]
 * @brief Internal macro implementation (auto-generated)
 * 
 * DO NOT USE THESE MACROS DIRECTLY
 * All macros in this file are prefixed with _ to indicate they are internal.
 * Use the public API macros defined in trlc/macro.hpp instead.
 * 
 * This file is automatically generated from a Python script.
 * 
 * If you wish to change the number of recursive argument expansions or the output directory,
 * please rerun the script and replace this file with the newly generated version.
 * 
 * Note: Modifications made directly to this file will be overwritten
 * the next time the script is executed.
 * 
 * To run the script, use the following command:
 * 
 *     python3 generate_variadic_macros.py <N> [directory]
 * 
 * Where <N> is the number of recursive expansions you want (e.g., 125).
 * [directory] is optional and specifies the output directory for the generated files.
 */
#pragma once

// clang-format off
"""

    footer = """
// clang-format on
"""

    # Combine all parts of the generated macros
    count_args_file = f"""
{header}

/**
 * @brief Count the number of variadic arguments (internal implementation)
 * 
 * Usage: _TRLC_COUNT(arg1, arg2, arg3, ...) -> returns the count
 * 
 * Example:
 *   _TRLC_COUNT(a, b, c) -> 3
 *   _TRLC_COUNT(x) -> 1
 * 
 * Note: Use TRLC_COUNT() from macro.hpp instead of this internal version.
 */
{count_args_macro}
{count_args_impl_macro}
{footer}
"""

    apply_args_file = f"""
{header}
#include "count.hpp"

/**
 * @brief Apply a macro to each argument with semicolon separation (internal implementation)
 * 
 * Usage: _TRLC_FOREACH_STMT(MACRO, arg1, arg2, ...) -> MACRO(arg1); MACRO(arg2); ...
 * 
 * Example:
 *   #define PRINT(x) std::cout << #x << std::endl
 *   _TRLC_FOREACH_STMT(PRINT, a, b, c)
 *   // Expands to: PRINT(a); PRINT(b); PRINT(c);
 * 
 * Use case: Generating statements for each argument
 * 
 * Note: Use TRLC_FOREACH_STMT() from macro.hpp instead of this internal version.
 */
{apply_macros}

// Recursive apply of arguments based on count (1 to {n})
{apply_impl_macros}
{footer}
"""

    apply_args_with_index_file = f"""
{header}
#include "count.hpp"

/**
 * @brief Apply a macro to each argument with index (internal implementation)
 * 
 * Usage: _TRLC_FOREACH_STMT_INDEXED(MACRO, arg1, arg2, ...) -> MACRO(arg1, 0); MACRO(arg2, 1); ...
 * 
 * Example:
 *   #define PRINT_INDEXED(x, i) std::cout << i << ": " << #x << std::endl
 *   _TRLC_FOREACH_STMT_INDEXED(PRINT_INDEXED, a, b, c)
 *   // Expands to: PRINT_INDEXED(a, 0); PRINT_INDEXED(b, 1); PRINT_INDEXED(c, 2);
 * 
 * Use case: When you need to know the position of each argument (0-based index)
 * 
 * Note: Use TRLC_FOREACH_STMT_INDEXED() from macro.hpp instead of this internal version.
 */
{apply_with_index_macros}

// Recursive apply with index based on count (1 to {n})
{apply_impl_macros_with_index}
{footer}
"""

    expand_args_file = f"""
{header}
#include "count.hpp"

/**
 * @brief Expand a macro for each argument with comma separation (internal implementation)
 * 
 * Usage: _TRLC_FOREACH_EXPR(MACRO, arg1, arg2, ...) -> MACRO(arg1), MACRO(arg2), ...
 * 
 * Example:
 *   #define TYPE(x) decltype(x)
 *   _TRLC_FOREACH_EXPR(TYPE, a, b, c)
 *   // Expands to: TYPE(a), TYPE(b), TYPE(c)
 * 
 * Use case: Generating comma-separated lists (arrays, function arguments, template parameters)
 * 
 * Note: Use TRLC_FOREACH_EXPR() from macro.hpp instead of this internal version.
 */
{expand_macros}

// Recursive expansion of arguments based on count (1 to {n})
{expand_impl_macros}
{footer}
"""

    append_args_file = f"""
{header}
#include "count.hpp"

/**
 * @brief Concatenate all arguments with delimiter then apply macro (internal implementation)
 * 
 * Usage: _TRLC_CONCAT(MACRO, delim, arg1, arg2, ...) -> MACRO(arg1##delim##arg2##delim##...)
 * 
 * Example:
 *   #define MAKE_ID(x) constexpr auto x = #x
 *   _TRLC_CONCAT(MAKE_ID, _, foo, bar, baz)
 *   // Expands to: MAKE_ID(foo_bar_baz)
 *   // Result: constexpr auto foo_bar_baz = "foo_bar_baz"
 * 
 * Use case: Creating compound identifiers or concatenated names
 * 
 * Note: Use TRLC_CONCAT() from macro.hpp instead of this internal version.
 */
{append_macros}

// Recursive appending of arguments based on count (1 to {n})
{append_impl_macros}
{footer}
"""

    expand_args_with_index_file = f"""
{header}
#include "count.hpp"

/**
 * @brief Expand a macro for each argument with index (internal implementation)
 * 
 * Usage: _TRLC_FOREACH_EXPR_INDEXED(MACRO, arg1, arg2, ...) -> MACRO(arg1, 0), MACRO(arg2, 1), ...
 * 
 * Example:
 *   #define PAIR(x, i) std::pair<decltype(x), int>{{x, i}}
 *   _TRLC_FOREACH_EXPR_INDEXED(PAIR, a, b, c)
 *   // Expands to: PAIR(a, 0), PAIR(b, 1), PAIR(c, 2)
 * 
 * Use case: Building indexed data structures, enumeration with positions
 * 
 * Note: Use TRLC_FOREACH_EXPR_INDEXED() from macro.hpp instead of this internal version.
 */
{expand_with_index_macros}

// Recursive expansion with index based on count (1 to {n})
{expand_impl_macros_with_index}
{footer}
"""
    return (
        count_args_file,
        apply_args_file,
        append_args_file,
        apply_args_with_index_file,
        expand_args_file,
        expand_args_with_index_file,
    )


def write_to_file(n, directory):
    # Generate the different parts of macros
    (
        count_args_file,
        apply_args_file,
        append_args_file,
        apply_args_with_index_file,
        expand_args_file,
        expand_args_with_index_file,
    ) = generate_macros(n)

    # Generate index strings
    index_strings_file = generate_index_strings(n)

    # Write COUNT macros to a separate file
    with open(f"{directory}/count.hpp", "w") as f:
        f.write(count_args_file)

    # Write FOREACH_STMT macros to a separate file
    with open(f"{directory}/foreach_stmt.hpp", "w") as f:
        f.write(apply_args_file)

    # Write FOREACH_STMT indexed macros to a separate file
    with open(f"{directory}/foreach_stmt_indexed.hpp", "w") as f:
        f.write(apply_args_with_index_file)

    # Write FOREACH_EXPR macros to a separate file
    with open(f"{directory}/foreach_expr.hpp", "w") as f:
        f.write(expand_args_file)

    # Write FOREACH_EXPR indexed macros to a separate file
    with open(f"{directory}/foreach_expr_indexed.hpp", "w") as f:
        f.write(expand_args_with_index_file)

    # Write CONCAT macros to a separate file
    with open(f"{directory}/concat.hpp", "w") as f:
        f.write(append_args_file)

    # Write index strings to a separate file
    with open(f"{directory}/index_strings.hpp", "w") as f:
        f.write(index_strings_file)

    print(
        f"Macros generated and written to {directory}/count.hpp, {directory}/foreach_stmt.hpp, "
        f"{directory}/foreach_stmt_indexed.hpp, {directory}/foreach_expr.hpp, "
        f"{directory}/foreach_expr_indexed.hpp, {directory}/concat.hpp, {directory}/index_strings.hpp"
    )


def main():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print(
            "Usage: python3 generate_variadic_macros.py <number_of_recursive_expansions> [directory]"
        )
        return

    try:
        num_expansions = int(sys.argv[1])
    except ValueError:
        print("Please provide a valid number for the recursive expansions.")
        return

    directory = sys.argv[2] if len(sys.argv) == 3 else "."

    write_to_file(num_expansions, directory)


if __name__ == "__main__":
    main()
