#pragma once

#include "enum/detail.hpp"
#include "enum/field.hpp" // IWYU pragma: export

#include "macro.hpp" // IWYU pragma: export

#include <cstdint>

namespace trlc
{
template<class Holder>

/**
 * @brief A structure that defines the default behavior and policies for enum handling.
 *
 * This struct encapsulates the necessary type definitions and policies used
 * by the TRLC framework for managing enumerations.
 *
 * Users can create their own custom EnumDef structures by defining a new
 * struct similar to this one. Custom EnumDefs can be passed to the
 * TRLC_ENUM_DETAIL macro to modify the behavior of enum management
 * in accordance with specific needs.
 */
struct DefaultEnumDef
{
    using holder = Holder;
    using value_type = int64_t;
    using value_search_policy = trlc::policy::BinarySearchPolicy;
    using name_search_policy = trlc::policy::CaseSensitiveStringSearchPolicy;
    using unknown_policy = trlc::policy::UnknownPolicy;
    using enum_type = trlc::Enum<value_type, holder>;
    using iterator = trlc::EnumIterator<holder>;
};

/**
 * @brief Macro to define a Default TRLC Enum with a given name and an arbitrary number of enumeration values.
 *
 * This macro expands into a call to a detailed macro that performs the actual enum definition logic.
 *
 * @param enumname The name of the enum being defined.
 * @param ... A variable number of enumeration values, which can include:
 *   - Normal enum values
 *   - TRLC_FIELD directives for value and descriptions,
 *     allowing for rich metadata associated with each enum value.
 *
 * @example
 * Example usage of the TRLC_ENUM macro to define an enumeration.
 *
 * TRLC_ENUM(Colors,
 *         RED,
 *         GREEN = TRLC_FIELD(value = 1),
 *         BLUE = TRLC_FIELD(desc = "Sky color"),
 *         YELLOW = TRLC_FIELD(value = 2, desc = "Color of ripe bananas."))
 * In this example, we define an enumeration called Colors with several values:
 * - RED: A regular enumerator with no associated metadata.
 * - GREEN: Assigned a value of 1.
 * - BLUE: Has a description indicating it is the color of the sky.
 * - YELLOW: Assigned a value of 2 and a description relating to ripe bananas.
 */
#define TRLC_ENUM(enumname, ...) \
    TRLC_ENUM_DETAIL(enumname, trlc::DefaultEnumDef, __VA_ARGS__)

} // namespace trlc