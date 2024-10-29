import sys


def generate_macros(n):
    # Generate GET_ARG macros
    get_arg_macros = ""
    for i in range(1, n + 1):
        args = (
            ", ".join([f"arg{j}" for j in range(1, i + 1)]) + ", ..."
            if i > 1
            else "arg1, ..."
        )
        get_arg_macros += f"#define TRLC_GET_ARG_{i}({args}) arg{i}\n"

    # Generate COUNT_ARGS macros
    count_args_macro = f"#define TRLC_COUNT_ARGS(...) TRLC_COUNT_ARGS_IMPL(__VA_ARGS__, {', '.join(map(str, range(n, 0, -1)))})"
    count_args_impl_macro = f"#define TRLC_COUNT_ARGS_IMPL({', '.join([f'_{i}' for i in range(1, n + 1)])}, N, ...) N"

    # Generate recursive APPLY_* macros
    apply_impl_macros = ""
    for i in range(1, n + 1):
        args = ", ".join([f"arg{j}" for j in range(1, i + 1)])
        apply_impl_macros += f"#define TRLC_APPLY_{i}(MACRO, {args}) "
        apply_impl_macros += "".join([f"MACRO(arg{j}); " for j in range(1, i + 1)])
        apply_impl_macros += "\n"

    # Generate APPLY and recursive expansion macros with indexing
    apply_impl_macros_with_index = "\n".join(
        [
            f"#define TRLC_APPLY_WITH_INDEX_{i}(MACRO, "
            + ", ".join([f"arg{j}" for j in range(1, i + 1)])
            + ") "
            + (
                "MACRO(arg1, 1); "
                + "".join([f"MACRO(arg{j}, {j}); " for j in range(2, i + 1)])
                if i > 1
                else "MACRO(arg1, 1);"
            )
            for i in range(1, n + 1)
        ]
    )

    # Generate recursive EXPAND_* macros
    expand_impl_macros = ""
    for i in range(1, n + 1):
        args = ", ".join([f"arg{j}" for j in range(1, i + 1)])
        expand_impl_macros += f"#define TRLC_EXPAND_{i}(MACRO, {args}) "

        # Use a generator expression to join MACRO calls without a trailing comma
        expand_impl_macros += ", ".join([f"MACRO(arg{j})" for j in range(1, i + 1)])
        expand_impl_macros += "\n"

    # Generate EXPAND and recursive expansion macros with indexing
    expand_impl_macros_with_index = "\n".join(
        [
            f"#define TRLC_EXPAND_WITH_INDEX_{i}(MACRO, "
            + ", ".join([f"arg{j}" for j in range(1, i + 1)])
            + ") "
            + (
                "MACRO(arg1, 1)"
                + "".join([f", MACRO(arg{j}, {j})" for j in range(2, i + 1)])
                if i > 1
                else "MACRO(arg1, 1)"
            )
            for i in range(1, n + 1)
        ]
    )

    # Generate recursive APPEND_* macros
    append_impl_macros = ""
    for i in range(1, n + 1):
        args = ",".join([f"arg{j}" for j in range(1, i + 1)])
        append_impl_macros += f"#define TRLC_APPEND_{i}(MACRO, delim, {args}) "

        # Use a generator expression to join MACRO calls without a trailing comma
        append_impl_macros += "MACRO(" + "##delim##".join([f"arg{j}" for j in range(1, i + 1)])
        append_impl_macros += ")\n"

    # Generate APPLY macros
    apply_macros = "#define TRLC_APPLY(MACRO, ...) TRLC_APPLY_IMPL(MACRO, TRLC_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)\n"
    apply_macros += "#define TRLC_APPLY_IMPL(MACRO, COUNT, ...) TRLC_APPLY_IMPL2(MACRO, COUNT, __VA_ARGS__)\n"
    apply_macros += "#define TRLC_APPLY_IMPL2(MACRO, COUNT, ...) TRLC_APPLY_##COUNT(MACRO, __VA_ARGS__)"

    # Generate EXPAND macros
    expand_macros = "#define TRLC_EXPAND(MACRO, ...) TRLC_EXPAND_IMPL(MACRO, TRLC_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)\n"
    expand_macros += "#define TRLC_EXPAND_IMPL(MACRO, COUNT, ...) TRLC_EXPAND_IMPL2(MACRO, COUNT, __VA_ARGS__)\n"
    expand_macros += "#define TRLC_EXPAND_IMPL2(MACRO, COUNT, ...) TRLC_EXPAND_##COUNT(MACRO, __VA_ARGS__)"

    # Generate APPEND macros
    append_macros = "#define TRLC_APPEND(MACRO, delim,...) TRLC_APPEND_IMPL(MACRO, delim, TRLC_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)\n"
    append_macros += "#define TRLC_APPEND_IMPL(MACRO, delim, COUNT, ...) TRLC_APPEND_IMPL2(MACRO, delim, COUNT, __VA_ARGS__)\n"
    append_macros += "#define TRLC_APPEND_IMPL2(MACRO, delim, COUNT,...) TRLC_APPEND_##COUNT(MACRO, delim, __VA_ARGS__)"

    # Generate APPLY_WITH_INDEX macros
    apply_with_index_macros = "#define TRLC_APPLY_WITH_INDEX(MACRO, ...) TRLC_APPLY_WITH_INDEX_IMPL(MACRO, TRLC_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)\n"
    apply_with_index_macros += "#define TRLC_APPLY_WITH_INDEX_IMPL(MACRO, COUNT, ...) TRLC_APPLY_WITH_INDEX_IMPL2(MACRO, COUNT, __VA_ARGS__)\n"
    apply_with_index_macros += "#define TRLC_APPLY_WITH_INDEX_IMPL2(MACRO, COUNT, ...) TRLC_APPLY_WITH_INDEX_##COUNT(MACRO, __VA_ARGS__)"

    # Generate EXPAND_WITH_INDEX macros
    expand_with_index_macros = "#define TRLC_EXPAND_WITH_INDEX(MACRO, ...) TRLC_EXPAND_WITH_INDEX_IMPL(MACRO, TRLC_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)\n"
    expand_with_index_macros += "#define TRLC_EXPAND_WITH_INDEX_IMPL(MACRO, COUNT, ...) TRLC_EXPAND_WITH_INDEX_IMPL2(MACRO, COUNT, __VA_ARGS__)\n"
    expand_with_index_macros += "#define TRLC_EXPAND_WITH_INDEX_IMPL2(MACRO, COUNT, ...) TRLC_EXPAND_WITH_INDEX_##COUNT(MACRO, __VA_ARGS__)"

    header = """
/**
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
 *     python3 macro_expansion_generator.py <N> [directory]
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

// Macro to get the number of arguments
{count_args_macro}
{count_args_impl_macro}
{footer}
"""

    apply_args_file = f"""
{header}
#include "count_args.hpp"

// Macro to apply a function to each argument
{apply_macros}

// Recursive apply of arguments based on count
{apply_impl_macros}
{footer}
"""

    apply_args_with_index_file = f"""
{header}
#include "count_args.hpp"

// Macro to apply a function to each argument with index
{apply_with_index_macros}

// Recursive apply of arguments based on count with index
{apply_impl_macros_with_index}
{footer}
"""

    expand_args_file = f"""
{header}
#include "count_args.hpp"

// Macro to expand a expr to each argument
{expand_macros}

// Recursive expansion of arguments based on count
{expand_impl_macros}
{footer}
"""

    append_args_file = f"""
{header}
#include "count_args.hpp"

// Macro to append a expr to each argument
{append_macros}

// Recursive appending of arguments based on count
{append_impl_macros}
{footer}
"""

    expand_args_with_index_file = f"""
{header}
#include "count_args.hpp"

// Macro to expand a expr to each with index
{expand_with_index_macros}

// Recursive expansion of arguments based on count with index
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

    # Write COUNT_ARGS macros to a separate file
    with open(f"{directory}/count_args.hpp", "w") as f:
        f.write(count_args_file)

    # Write APPLY macros to a separate file
    with open(f"{directory}/apply_args.hpp", "w") as f:
        f.write(apply_args_file)

    # Write APPLY macros with index to a separate file
    with open(f"{directory}/apply_args_with_index.hpp", "w") as f:
        f.write(apply_args_with_index_file)

    # Write EXPAND macros to a separate file
    with open(f"{directory}/expand_args.hpp", "w") as f:
        f.write(expand_args_file)

    # Write EXPAND macros with index to a separate file
    with open(f"{directory}/expand_args_with_index.hpp", "w") as f:
        f.write(expand_args_with_index_file)

    # Write APPEND macros to a separate file
    with open(f"{directory}/append_args.hpp", "w") as f:
        f.write(append_args_file)

    print(
        f"Macros generated and written to {directory}/count_args.hpp, {directory}/apply_args.hpp, "
        f"{directory}/apply_args_with_index.hpp, {directory}/expand_args.hpp, "
        f"{directory}/expand_args_with_index.hpp",
        f"{directory}/append_args.hpp",
    )


def main():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print(
            "Usage: python3 macro_expansion_generator.py <number_of_recursive_expansions> [directory]"
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
