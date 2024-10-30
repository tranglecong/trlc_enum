#pragma once

#include "enum/detail.hpp"
#include "enum/field.hpp" // IWYU pragma: export

#include "macro.hpp" // IWYU pragma: export

#include <cstdint>

namespace trlc
{
template<class Holder>
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

#define TRLC_ENUM(enumname, ...) \
    TRLC_ENUM_DETAIL(enumname, trlc::DefaultEnumDef, __VA_ARGS__)

} // namespace trlc