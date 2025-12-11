#pragma once

#include <array> 
#include <string_view>

#include "trlc/enums/field/field_parser.hpp"
#include "trlc/enums/field/field_utils.hpp"

#define TRLC_INTERNAL_DEFAULT_FIELDS(_ENUM_NAME)                         \
    [[maybe_unused]] inline static constexpr NameType name{#_ENUM_NAME}; \
    [[maybe_unused]] inline static constexpr ValueType value{            \
        trlc::enums::field::defaultValue<ValueType>()};                  \
    [[maybe_unused]] inline static constexpr DescType desc{};            \
    [[maybe_unused]] inline static constexpr ExtraType extra{};

#define TRLC_INTERNAL_ENUM_META_LAMBDA(_INPUT, _INDEX)                            \
    []() constexpr -> EnumData {                                                  \
        EnumData _INPUT;                                                          \
        EnumData result = _INPUT;                                                 \
        std::string_view name{trlc::enums::field::extractName(#_INPUT)};          \
        return (EnumData{_INDEX, name, result.value, result.desc, result.extra}); \
    }()

#define TRLC_INTERNAL_BUILD_ENUM_META_LIST(...) \
    TRLC_FOREACH_EXPR_INDEXED(TRLC_INTERNAL_ENUM_META_LAMBDA, __VA_ARGS__)

#define TRLC_INTERNAL_BUILD_ENUM_DATAS(...)                                      \
    inline static constexpr auto datas{                                          \
        trlc::enums::field::fillEnumValues<EnumData, SIZE, Policies::Increment>( \
            {TRLC_INTERNAL_BUILD_ENUM_META_LIST(__VA_ARGS__)})};

#define TRLC_INTERNAL_ENUM_CONSTANT(_INPUT, _INDEX)               \
    [[maybe_unused]] inline static constexpr Enum _INPUT = Enum { \
        _INDEX                                                    \
    }

#define TRLC_INTERNAL_ENUM_CONSTANTS_IMPL(...) \
    TRLC_FOREACH_STMT_INDEXED(TRLC_INTERNAL_ENUM_CONSTANT, __VA_ARGS__)

#define TRLC_INTERNAL_ENUM_CONSTANTS(...) TRLC_INTERNAL_ENUM_CONSTANTS_IMPL(__VA_ARGS__)

#define TRLC_INTERNAL_SORTED_INDICES()                            \
    [[maybe_unused]] inline static constexpr auto sorted_indices{ \
        trlc::enums::field::sortedIndices(datas)};