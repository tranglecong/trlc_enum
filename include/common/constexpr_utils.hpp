#pragma once

#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <string_view>

namespace trlc
{
namespace constexpr_utils
{

/**
 * @brief Checks if a substring is contained within a string view.
 *
 * This function checks if the provided substring exists within the given string view.
 * It uses the `find` method to determine if the substring is present.
 *
 * @param str The string view in which to search for the substring.
 * @param substring The string view representing the substring to search for.
 * @return true If the substring is found within the string view.
 * @return false If the substring is not found within the string view.
 *
 * @note This function is `constexpr`, allowing it to be evaluated at compile time if provided
 *       with constant expressions.
 */
constexpr bool contains(std::string_view str, std::string_view substring)
{
    return str.find(substring) != std::string_view::npos;
}

/**
 * @brief Checks if all elements in the provided array are unique at compile time.
 *
 * This function iterates over the given array and checks for duplicate elements
 * using nested loops. It returns true if all elements are unique, and false otherwise.
 *
 * @tparam T The type of elements in the array.
 * @tparam N The size of the array.
 * @param arr The array to check for uniqueness.
 * @return true if all elements in the array are unique.
 * @return false if there are any duplicates in the array.
 *
 * @note This function is constexpr, allowing for compile-time evaluation when possible.
 */
template<typename T, std::size_t N>
constexpr bool has_unique_elements(const std::array<T, N>& arr)
{
    for (std::size_t i = 0; i < N; ++i)
    {
        for (std::size_t j = i + 1; j < N; ++j)
        {
            if (arr[i] == arr[j])
            {
                return false; // found a duplicate
            }
        }
    }
    return true; // all elements are unique
}

/**
 * @brief Checks if a character is a digit ('0' through '9').
 *
 * This function determines whether the given character 'c'
 * is within the range of characters that represent decimal digits.
 * It returns true if the character is a digit and false otherwise.
 *
 * @param c The character to be checked.
 * @return true if 'c' is a digit; false otherwise.
 */
constexpr bool isDigit(char c)
{
    return (c >= '0' && c <= '9');
}

/**
 * @brief Converts a string view to a signed integer (std::int64_t).
 *
 * This function parses the input string view to produce a signed integer.
 * It handles optional leading signs ('-' for negative and '+' for positive).
 * If the input is invalid (contains non-numeric characters), it returns std::nullopt.
 *
 * @param str The string view containing the number to be converted.
 * @param pos Optional pointer to a size_t where the position after the parsed number will be stored.
 *            If provided, it can be used to determine where parsing stopped in the input string.
 * @return std::optional<std::int64_t> The parsed integer if successful, or std::nullopt if the input is invalid.
 *
 * @note The conversion is performed in a manner that allows evaluation at compile time
 *       if called with a constant string view.
 */
/**
 * @brief Converts a string view to a signed integer (std::int64_t),
 *        after removing all non-numeric characters from the beginning and end.
 *
 * This function parses the input string view to produce a signed integer.
 * It handles leading signs ('-' for negative and '+' for positive) and
 * ignores non-numeric characters before and after the main numeric part.
 * If the input is invalid (contains no valid numeric characters), it returns std::nullopt.
 *
 * @param str The string view containing the number to be converted.
 * @param pos Optional pointer to a size_t where the position after the parsed number will be stored.
 *            If provided, it can be used to determine where parsing stopped in the input string.
 * @return std::optional<std::int64_t> The parsed integer if successful, or std::nullopt if the input is invalid.
 *
 * @note The conversion is performed in a manner that allows evaluation at compile time
 *       if called with a constant string view.
 */
constexpr std::optional<std::int64_t> stoi(std::string_view str, std::size_t* pos = nullptr)
{
    using namespace std::literals;

    // Find the first digit or sign
    std::size_t start = 0;
    while (start < str.size() && !isDigit(str[start]) && str[start] != '-' && str[start] != '+')
    {
        ++start; // Skip non-numeric characters
    }

    // If we've skipped all characters, return nullopt
    if (start == str.size())
    {
        return std::nullopt;
    }

    bool isNegative = str[start] == '-';
    if (str[start] == '+' || str[start] == '-')
    {
        ++start; // Move past the sign
    }

    std::int64_t result = 0;

    // Parse digits
    while (start < str.size() && isDigit(str[start]))
    {
        char c = str[start++];

        // Check for overflow before multiplying
        if (result > (std::numeric_limits<std::int64_t>::max() - (c - '0')) / 10)
        {
            return std::nullopt; // Overflow occurred
        }

        result = result * 10 + (c - '0');
    }

    // Apply the sign
    if (isNegative)
    {
        result = -result;
    }

    // Skip any trailing non-numeric characters
    while (start < str.size() && !isDigit(str[start]))
    {
        ++start;
    }

    if (pos != nullptr)
    {
        *pos = start; // position is the end of the parsed number
    }

    return result;
}

} // namespace constexpr_utils

} // namespace trlc
