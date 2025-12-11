#pragma once

#include <string_view>

#include "trlc/enums/enum.hpp"
#include "trlc/enums/field/field.hpp"
#include "trlc/enums/iteration.hpp"
#include "trlc/enums/policies.hpp"

#define TRLC_INTERNAL_TYPEDEFS(_ENUM_NAME, _POLICIES)          \
    using Policies = _POLICIES;                                \
    using NameType = std::string_view;                         \
    using ValueType = typename Policies::ValueType;            \
    using DescType = typename Policies::DescType;              \
    using ExtraType = typename Policies::ExtraType;            \
    using EnumData = trlc::enums::field::EnumData<_ENUM_NAME>; \
    using Enum = trlc::enums::Enum<_ENUM_NAME>;

#define TRLC_INTERNAL_ENUM_NAME(_ENUM_NAME) \
    [[maybe_unused]] inline static constexpr std::string_view NAME = #_ENUM_NAME;

#define TRLC_INTERNAL_SIZE(...) inline static constexpr std::size_t SIZE = TRLC_COUNT(__VA_ARGS__);

#define TRLC_INTERNAL_FROM_VALUE()                                                        \
    template <typename T>                                                                 \
    inline static constexpr auto fromValue(const T& val)                                  \
        -> std::enable_if_t<std::is_constructible_v<ValueType, T>, std::optional<Enum>> { \
        std::optional<std::size_t> result{Policies::Search::searchByValue(datas, val)};   \
        if (!result.has_value()) {                                                        \
            return std::nullopt;                                                          \
        }                                                                                 \
        return Enum{result.value()};                                                      \
    }

#define TRLC_INTERNAL_FROM_NAME()                                                          \
    template <typename T = std::string_view>                                               \
    inline static constexpr auto fromName(const T& element_name)                           \
        -> std::enable_if_t<std::is_constructible_v<NameType, T>, std::optional<Enum>> {   \
        using NameSearch =                                                                 \
            trlc::enums::policies::LinearSearchPolicy<NameType, Policies::CASE_SENSITIVE>; \
        std::optional<std::size_t> result{NameSearch::searchByName(datas, element_name)};  \
        if (!result.has_value()) {                                                         \
            return std::nullopt;                                                           \
        }                                                                                  \
        return Enum{result.value()};                                                       \
    }

#define TRLC_INTERNAL_ITERATION()                                     \
    struct Range {                                                    \
        constexpr auto begin() const -> trlc::enums::Iterator<Enum> { \
            return trlc::enums::Iterator<Enum>{0};                    \
        }                                                             \
        constexpr auto end() const -> trlc::enums::Iterator<Enum> {   \
            return trlc::enums::Iterator<Enum>{SIZE};                 \
        }                                                             \
    };                                                                \
                                                                      \
    static constexpr auto all() -> Range {                            \
        return {};                                                    \
    }                                                                 \
                                                                      \
    static constexpr auto begin() -> trlc::enums::Iterator<Enum> {    \
        return trlc::enums::Iterator<Enum>{0};                        \
    }                                                                 \
                                                                      \
    static constexpr auto end() -> trlc::enums::Iterator<Enum> {      \
        return trlc::enums::Iterator<Enum>{SIZE};                     \
    }