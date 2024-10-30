
#pragma once

#include "trlc/constexpr_utils.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace trlc
{
namespace enum_feild
{

/**
 * @brief A helper struct to normalize values or perform operations.
 */
struct NormalizeHelper
{
    /**
     * @brief An operator() overload that returns a new NormalizeHelper instance.
     *
     * This operator is constexpr and takes a pointer to a character string.
     *
     * @param str A pointer to a character string.
     * @return A new instance of NormalizeHelper.
     */
    constexpr NormalizeHelper operator()(const char*) const
    {
        return NormalizeHelper{};
    }

    /**
     * @brief An operator* overload that returns index.
     *
     * Helper for create enum by TRLC_ENUM for FIELD case.
     *
     * @param value An size_t value.
     * @return The index of tracking enum array.
     */
    constexpr size_t operator*(size_t value) const
    {
        return value;
    }
};

/**
 * @brief Retrieves the substring of a string_view before the first equal sign.
 *
 * If an equal sign is found, it returns the substring before it. Otherwise, it returns the whole string_view.
 *
 * @param str The input string_view to analyze.
 * @return A string_view representing the substring before the equal sign or the whole input if no equal sign is found.
 */
constexpr std::string_view before_equal(std::string_view str)
{
    auto pos = str.find('=');
    return pos != std::string_view::npos ? str.substr(0, pos) : str;
}

/**
 * @brief Extracts a numeric value from a specified field in the input string.
 *
 * Searches for the field and retrieves its associated numeric value if it exists.
 *
 * @param field The field name to search for.
 * @param input The input string_view containing field information.
 * @return An optional string_view representing the numeric value found or std::nullopt if not found.
 */
constexpr std::optional<std::string_view> trlc_field_numberic(std::string_view field, std::string_view input)
{
    auto default_pos = input.find(field);
    if (default_pos == std::string_view::npos)
    {
        return std::nullopt;
    }
    default_pos += field.size();
    auto equal_pos = input.find('=', default_pos);
    if (equal_pos == std::string_view::npos)
    {
        return std::nullopt;
    }
    equal_pos++;

    // Find the end of the default value
    auto end_of_default = input.find('_', default_pos);
    if (end_of_default == std::string_view::npos)
    {
        end_of_default = input.find('\"', default_pos);
    }
    if (end_of_default == std::string_view::npos)
    {
        return std::nullopt;
    }
    return input.substr(equal_pos, end_of_default - equal_pos);
}

/**
 * @brief Extracts a string value from a specified field in the input string.
 *
 * Similar to trlc_field_numberic, but designed to extract string values and handle escaping.
 *
 * @param field The field name to search for.
 * @param input The input string_view containing field information.
 * @return An optional string_view representing the string value found or std::nullopt if not found.
 */
constexpr std::optional<std::string_view> trlc_field_string(std::string_view field, std::string_view input)
{
    auto default_pos = input.find(field);
    if (default_pos == std::string_view::npos)
    {
        return std::nullopt;
    }
    default_pos += field.size();
    auto equal_pos = input.find('=', default_pos);
    if (equal_pos == std::string_view::npos)
    {
        return std::nullopt;
    }
    equal_pos++;

    // Find the end of the default value
    auto end_of_default = input.find('_', default_pos);
    if (end_of_default == std::string_view::npos)
    {
        end_of_default = input.find('\"', default_pos);
    }
    if (end_of_default == std::string_view::npos)
    {
        return std::nullopt;
    }

    std::string_view result{input.substr(equal_pos, end_of_default - equal_pos)};
    size_t start = result.find_first_not_of("\\\"");
    size_t end = result.find_first_of("\\\"", start + 3);

    if (start == std::string_view::npos)
    {
        return std::string_view{};
    }
    return std::string_view(result.substr(start + 3, end - start - 3));
}

/**
 * @brief Searches for a given value in a constexpr std::array.
 *
 * This function performs a linear search on the provided `std::array`
 * and returns true if the value is found, or false otherwise.
 *
 * @tparam T The type of the array elements.
 * @tparam N The size of the array.
 * @param arr The `std::array` to search.
 * @param value The value to find within the array.
 * @return constexpr bool True if the value is found, false otherwise.
 */
template<typename T, std::size_t N>
constexpr bool find_value(const std::array<T, N>& arr, const T value)
{
    for (std::size_t i = 0; i < N; ++i)
    {
        if (arr[i] == value)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Creates an array of indices based on input parameters for specific values.
 *
 * This function looks for occurrences of "NormalizeHelper" in the provided arguments,
 * fetching their default values and returning the constructed indices.
 *
 * @tparam TrLc enum Holder class
 * @tparam N The number of elements in the args array.
 * @param args An array of const character pointers.
 * @return An array of Holder::value_type representing the constructed indices.
 * @throws std::invalid_argument If any default values are invalid.
 */
template<class Holder, size_t N>
constexpr auto array_values(const char* const (&args)[N])
{
    std::array<typename Holder::value_type, N> indices{};
    std::array<typename Holder::value_type, N> specific_values{};
    constexpr auto maximum_value{std::numeric_limits<typename Holder::value_type>::max()};
    constexpr auto minimum_value{std::numeric_limits<typename Holder::value_type>::min()};
    for (size_t index{0}; index < N; ++index)
    {
        indices[index] = maximum_value;
        specific_values[index] = maximum_value;
    }
    for (size_t index{0}; index < N; ++index)
    {
        if (trlc::constexpr_utils::contains(args[index], "NormalizeHelper"))
        {
            const auto default_value_str{trlc::enum_feild::trlc_field_numberic("default", args[index])};
            if (!default_value_str)
            {
                continue;
            }

            const auto raw_default_value{trlc::constexpr_utils::stoi(default_value_str.value())};
            if (!raw_default_value)
            {
                throw std::invalid_argument("Invalid default value!");
            }

            const auto default_value{raw_default_value.value()};
            if (default_value < minimum_value || default_value > maximum_value)
            {
                throw std::invalid_argument("Default value overflow with current enum value type!");
            }

            indices[index] = default_value;
        }
    }

    if (!trlc::constexpr_utils::has_unique_elements(specific_values))
    {
        throw std::invalid_argument("Value must be unique!");
    }

    typename Holder::value_type current{0};

    for (size_t index{0}; index < N; ++index)
    {
        if (indices[index] == maximum_value)
        {
            for (size_t loop{0}; loop < N; ++loop)
            {
                if (find_value(specific_values, current))
                {
                    current++;
                    continue;
                }
                break;
            }
            indices[index] = current;
            current++;
        }
    }
    return indices;
}

/**
 * @brief Extracts names from an array of input character pointers.
 *
 * This function retrieves the name associated with "NormalizeHelper"
 * from the input arguments, trimming whitespace as needed.
 *
 * @tparam N The number of elements in the args array.
 * @param args An array of const character pointers.
 * @return An array of string_view representing the extracted names.
 */
template<size_t N>
constexpr auto create_array_name(const char* const (&args)[N])
{
    std::array<std::string_view, N> names{};
    for (size_t index{0}; index < N; ++index)
    {
        if (trlc::constexpr_utils::contains(args[index], "NormalizeHelper"))
        {
            std::string_view name = before_equal(args[index]);

            size_t start = name.find_first_not_of(" ");
            size_t end = name.find_last_not_of(" ");

            names[index] = name.substr(start, end - start + 1);
            continue;
        }
        names[index] = args[index];
    }

    return names;
}

/**
 * @brief Extracts descriptions from an array of input character pointers.
 *
 * This function retrieves the description associated with "NormalizeHelper"
 * from the input arguments, returning empty views where no description is found.
 *
 * @tparam N The number of elements in the args array.
 * @param args An array of const character pointers.
 * @return An array of string_view representing the extracted descriptions.
 */
template<size_t N>
constexpr auto create_array_description(const char* const (&args)[N])
{
    std::array<std::string_view, N> descriptions{};
    for (size_t index{0}; index < N; ++index)
    {
        if (trlc::constexpr_utils::contains(args[index], "NormalizeHelper"))
        {
            const auto description_str{trlc::enum_feild::trlc_field_string("desc", args[index])};
            if (description_str)
            {
                descriptions[index] = description_str.value();
                continue;
            }
        }
        descriptions[index] = std::string_view{};
    }

    return descriptions;
}

/**
 * @brief Finds the index of the next minimum value in the array that is greater than the given minimum.
 *
 * This function iterates through the provided array and returns the index of the first
 * element that is greater than the specified minimum value.
 *
 * @tparam T Type of the elements in the array.
 * @tparam N Size of the array.
 * @param array The array of elements to search through.
 * @param min The minimum value to compare against.
 * @return std::size_t The index of the next minimum value greater than `min`, or `N` if none is found.
 */
template<typename T, std::size_t N>
constexpr auto index_of_next_min(const std::array<T, N>& array, int64_t min)
{
    std::size_t next_min_index{N};
    int64_t next_min{std::numeric_limits<T>::max()};

    for (std::size_t index{0}; index < array.size(); ++index)
    {
        if (min >= static_cast<int64_t>(array[index]))
        {
            continue;
        }
        if (static_cast<int64_t>(array[index]) < next_min)
        {
            next_min = static_cast<int64_t>(array[index]);
            next_min_index = index;
        }
    }

    return next_min_index;
}

/**
 * @brief Enumerates an array of values into a specified holder type.
 *
 * This function creates an array of enum types based on the values, names, and descriptions provided
 * in the Holder template. It finds the index of the next valid minimum value for each enumeration.
 *
 * @tparam Holder A type that contains the necessary data:
 *                - `enum_type`: The type of the enumeration.
 *                - `m_size`: The size of the enumeration array.
 *                - `m_values`: Values to be used for enumeration.
 *                - `m_names`: Names corresponding to each value.
 *                - `m_descs`: Descriptions corresponding to each value.
 *
 * @return std::array<typename Holder::enum_type, Holder::m_size> An array of enumerated types created from the input holder.
 */
template<class Holder>
constexpr auto create_array_enum()
{
    std::array<typename Holder::enum_type, Holder::m_size> result{};
    std::int64_t min_value{std::numeric_limits<std::int64_t>::min()};

    for (size_t index{0}; index < Holder::m_size; ++index)
    {
        auto next_min_index{index_of_next_min(Holder::m_values, min_value)};
        min_value = static_cast<int64_t>(Holder::m_values[next_min_index]);
        result[index] = typename Holder::enum_type{Holder::m_values[next_min_index],
                                                   Holder::m_names[next_min_index],
                                                   Holder::m_descs[next_min_index]};
    }
    return result;
}

} // namespace enum_feild
} // namespace trlc
