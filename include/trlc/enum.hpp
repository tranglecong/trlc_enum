#pragma once

#include "trlc/enums/detail.hpp"

#pragma clang diagnostic ignored "-Wvariadic-macro-arguments-omitted"  // NOLINT

// Main macro: Define enum with custom policies
// Expands to a struct with:
// - Type definitions from policies
// - Constexpr arrays: names[], values[], descs[], extras[]
// - Sorted indices for binary search
// - Static constants: ENUM_NAME::VALUE1, VALUE2, ...
// - Lookup functions: fromValue(), fromName()
// - Iteration support: all(), begin(), end()
#define TRLC_ENUM_DEF(_ENUM_NAME, _POLICIES, ...)     \
    struct _ENUM_NAME {                               \
        TRLC_INTERNAL_TYPEDEFS(_ENUM_NAME, _POLICIES) \
        TRLC_INTERNAL_ENUM_NAME(_ENUM_NAME)           \
        TRLC_INTERNAL_SIZE(__VA_ARGS__)               \
        TRLC_INTERNAL_DEFAULT_FIELDS(_ENUM_NAME)      \
        TRLC_INTERNAL_BUILD_ENUM_DATAS(__VA_ARGS__)   \
        TRLC_INTERNAL_SORTED_INDICES()                \
        TRLC_INTERNAL_ENUM_CONSTANTS(__VA_ARGS__)     \
        TRLC_INTERNAL_FROM_VALUE()                    \
        TRLC_INTERNAL_FROM_NAME()                     \
        TRLC_INTERNAL_ITERATION()                     \
    }

// Convenient macro: Define enum with default policies (int values, binary search)
#define TRLC_ENUM(_ENUM_NAME, ...) \
    TRLC_ENUM_DEF(_ENUM_NAME, trlc::enums::policies::DefaultPolicies<>, __VA_ARGS__)