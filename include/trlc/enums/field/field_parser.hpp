#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <string_view>

#include "trlc/enums/field/field_utils.hpp"
#include "trlc/macro.hpp"

#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"  // NOLINT

namespace trlc::enums::field {

template <typename Manager>
struct EnumData {
    using NameType = typename Manager::NameType;
    using ValueType = typename Manager::ValueType;
    using DescType = typename Manager::DescType;
    using ExtraType = typename Manager::ExtraType;
    using EnumType = typename Manager::Enum;
    using ManagerType = Manager;

    std::size_t index{};
    NameType name{};
    ValueType value{[]() {
        if constexpr (std::is_arithmetic_v<ValueType>) {
            return std::numeric_limits<ValueType>::max();
        } else if constexpr (std::is_same_v<ValueType, std::string_view>) {
            return std::string_view("TRLC_VALUE_DEFAULT");
        } else {
            return ValueType{};
        }
    }()};
    DescType desc{};
    ExtraType extra{};

    constexpr EnumType operator=(EnumType enm) const { return enm; }  // NOLINT
};

}  // namespace trlc::enums::field

#define TRLC_INTERNAL_DECLARE_FIELD_VAR(_FIELD) [[maybe_unused]] constexpr auto _FIELD

#define TRLC_INTERNAL_DECLARE_FIELD_VARS(...) \
    TRLC_FOREACH_STMT(TRLC_INTERNAL_DECLARE_FIELD_VAR, __VA_ARGS__)

#define TRLC_INTERNAL_FIELD_DECL(...)                   \
    []() constexpr -> EnumData {                        \
        TRLC_INTERNAL_DECLARE_FIELD_VARS(__VA_ARGS__)   \
        return (EnumData{0, name, value, desc, extra}); \
    }()

#define TRLC_FIELD(...) TRLC_INTERNAL_FIELD_DECL(__VA_ARGS__)