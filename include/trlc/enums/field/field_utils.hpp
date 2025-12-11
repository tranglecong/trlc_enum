#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <string_view>
#include <type_traits>

#include "trlc/enums/policies/increment.hpp"

namespace trlc::enums::field {

/**
 * @brief Trim leading whitespace from string_view
 * @param sv Input string_view
 * @return Starting position after leading whitespace
 */
constexpr std::size_t trimLeadingWhitespace(std::string_view sv) {
    std::size_t start = 0;
    while (start < sv.size() && (sv[start] == ' ' || sv[start] == '\t')) {
        ++start;
    }
    return start;
}

/**
 * @brief Trim trailing whitespace from string_view
 * @param sv Input string_view
 * @param start Starting position
 * @param end Initial end position
 * @return End position before trailing whitespace
 */
constexpr std::size_t trimTrailingWhitespace(std::string_view sv,
                                             std::size_t start,
                                             std::size_t end) {
    while (end > start && (sv[end - 1] == ' ' || sv[end - 1] == '\t')) {
        --end;
    }
    return end;
}

/**
 * @brief Find position of a character in string_view
 * @param sv Input string_view
 * @param ch Character to find
 * @param start Starting position for search
 * @return Position of character or npos if not found
 */
constexpr std::size_t findCharacter(std::string_view sv, char ch, std::size_t start = 0) {
    return sv.find(ch, start);
}

/**
 * @brief Extract substring with trimmed whitespace
 * @param sv Input string_view
 * @param start Starting position
 * @param end Ending position
 * @return Trimmed substring
 */
constexpr std::string_view extractTrimmedSubstring(std::string_view sv,
                                                   std::size_t start,
                                                   std::size_t end) {
    end = trimTrailingWhitespace(sv, start, end);
    return sv.substr(start, end - start);
}

/**
 * @brief Extract name from different enum value formats
 *
 * Handles three cases:
 * 1. Simple name: "RED" -> "RED"
 * 2. Name with field: "SEDAN = TRLC_FIELD(...)" -> "SEDAN"
 * 3. Name with partial field: "WITH_DEFAULT = TRLC_FIELD(value = 5)" -> "WITH_DEFAULT"
 *
 * @param full Full enum value string
 * @return Extracted and trimmed name
 */
constexpr std::string_view extractName(std::string_view full) {
    // Trim leading whitespace
    std::size_t start = trimLeadingWhitespace(full);

    // Find '=' position
    std::size_t eq_pos = findCharacter(full, '=', start);

    // No '=' found - return trimmed input (case 1: RED, YELLOW, etc.)
    if (eq_pos == std::string_view::npos) {
        return extractTrimmedSubstring(full, start, full.size());
    }

    // '=' found - extract name before it (case 2 & 3)
    return extractTrimmedSubstring(full, start, eq_pos);
}

/**
 * @brief Get default value for a type T
 * For arithmetic types: max() indicates default
 * For string_view: "TRLC_VALUE_DEFAULT" indicates default
 * For custom types: default constructor indicates default
 * @tparam T Value type
 * @return Default value for type T
 */
template <typename T>
constexpr T defaultValue() {
    if constexpr (std::is_arithmetic_v<T>) {
        return std::numeric_limits<T>::max();
    } else if constexpr (std::is_same_v<T, std::string_view>) {
        return std::string_view("TRLC_VALUE_DEFAULT");
    } else {
        return T{};
    }
}

/**
 * @brief Check if value is default (unspecified)
 *
 * For arithmetic types: max() indicates default
 * For string_view: "TRLC_VALUE_DEFAULT" indicates default
 * For custom types: default constructor indicates default
 *
 * @tparam T Value type
 * @param value Value to check
 * @return true if value is default, false otherwise
 */
template <typename T>
constexpr bool isDefaultValue(T value) {
    if constexpr (std::is_arithmetic_v<T>) {
        return value == std::numeric_limits<T>::max();
    } else if constexpr (std::is_same_v<T, std::string_view>) {
        return value == "TRLC_VALUE_DEFAULT";
    } else {
        // For custom types, assume default constructor indicates default
        return value == T{};
    }
}

/**
 * @brief Check if array contains a specific non-default value
 *
 * Ignores default values in the check.
 *
 * @tparam EnumData Enum data type
 * @tparam N Array size
 * @param arr Array to search
 * @param value Value to find
 * @return true if value exists in array, false otherwise
 */
template <typename EnumData, std::size_t N>
constexpr bool hasValue(const std::array<EnumData, N>& arr, typename EnumData::ValueType value) {
    for (std::size_t i = 0; i < N; ++i) {
        if (!isDefaultValue(arr[i].value) && arr[i].value == value) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Check for duplicate non-default values in an array
 *
 * Ignores default values when checking for duplicates.
 *
 * @tparam EnumData Enum data type
 * @tparam N Array size
 * @param values Array to check
 * @return true if duplicates exist, false otherwise
 */
template <typename EnumData, std::size_t N>
constexpr bool hasDuplicateValues(const std::array<EnumData, N>& values) {
    for (std::size_t i = 0; i < N; ++i) {
        // Skip default values
        if (isDefaultValue(values[i].value)) {
            continue;
        }

        // Check if this value appears again later in the array
        for (std::size_t j = i + 1; j < N; ++j) {
            if (!isDefaultValue(values[j].value) && values[i].value == values[j].value) {
                return true;  // Found duplicate
            }
        }
    }
    return false;  // No duplicates
}

/**
 * @brief Fill default enum values using IncrementPolicy
 *
 * Fills in default (unspecified) enum values by incrementing from a base value,
 * while avoiding duplicates of already specified values.
 *
 * @tparam EnumData Enum data type
 * @tparam N Array size
 * @tparam Policy Increment policy to use
 * @param datas Input array of enum data
 * @return New array with filled enum values
 */
template <typename EnumData,
          std::size_t N,
          typename Policy = policies::DefaultIncrementPolicy<typename EnumData::ValueType>>
constexpr std::array<EnumData, N> fillEnumValues(const std::array<EnumData, N>& datas) {
    // Step 0: Check for duplicate explicit datas (compile-time error)
    assert(!hasDuplicateValues(datas));

    std::array<EnumData, N> result{datas};
    // Step 1: Determine base value (only from first element for arithmetic types)
    typename EnumData::ValueType auto_value = Policy::startValue();

    if constexpr (std::is_arithmetic_v<typename EnumData::ValueType>) {
        // For arithmetic types: use first element as base if it's explicit
        if (N > 0 && !isDefaultValue(result[0].value)) {
            auto_value = result[0].value;
        }
    }
    // For string_view and other types: always use Policy::startValue()

    // Step 2: Fill default datas, skipping already-used datas
    std::size_t fill_index = 0;

    for (std::size_t i = 0; i < N; ++i) {
        if (isDefaultValue(result[i].value)) {
            // Skip datas that already exist in the array (avoid duplicates)
            while (hasValue(result, auto_value)) {
                auto_value = Policy::increment(auto_value, fill_index);
            }

            result[i].value = auto_value;
            auto_value = Policy::increment(auto_value, fill_index);
            ++fill_index;
            continue;
        }
        auto_value = result[i].value;
    }
    return result;
}

/**
 * @brief Compare two values for sorting (works with arithmetic and string types)
 *
 * @tparam T Value type (int, float, string_view, etc.)
 * @param a First value
 * @param b Second value
 * @return true if a < b, false otherwise
 */
template <typename T>
constexpr bool isLessThan(T a, T b) {
    if constexpr (std::is_same_v<T, std::string_view>) {
        // Lexicographic comparison for string_view
        std::size_t min_len = (a.size() < b.size()) ? a.size() : b.size();
        for (std::size_t i = 0; i < min_len; ++i) {
            if (a[i] != b[i]) {
                return a[i] < b[i];
            }
        }
        return a.size() < b.size();
    } else {
        // For arithmetic and custom types with operator<
        return a < b;
    }
}

/**
 * @brief Swap two elements in an array
 *
 * @tparam T Element type
 * @tparam N Array size
 * @param arr Array to modify
 * @param i First index
 * @param j Second index
 */
template <typename T, std::size_t N>
constexpr void swap(std::array<T, N>& arr, std::size_t i, std::size_t j) {
    T temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

/**
 * @brief Get sorted indices of enum data array using insertion sort
 *
 * @tparam T EnumData type
 * @tparam N Array size
 * @param datas Array of EnumData
 * @return Array of sorted indices
 */

template <typename EnumData, std::size_t N>
constexpr std::array<std::size_t, N> sortedIndices(const std::array<EnumData, N>& datas) {
    // Initialize indices array: [0, 1, 2, ..., N-1]
    std::array<std::size_t, N> indices{};
    for (std::size_t i = 0; i < N; ++i) {
        indices[i] = i;
    }

    // Insertion sort on indices based on values
    for (std::size_t i = 1; i < N; ++i) {
        std::size_t key_idx = indices[i];
        typename EnumData::ValueType key_value{datas[key_idx].value};
        std::size_t j = i;

        // Shift elements that are greater than key_value
        while (j > 0 && isLessThan(key_value, datas[indices[j - 1]].value)) {
            indices[j] = indices[j - 1];
            --j;
        }
        indices[j] = key_idx;
    }

    return indices;
}

}  // namespace trlc::enums::field
