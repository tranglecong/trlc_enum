#pragma once

#include <cstddef>
#include <string_view>
#include <type_traits>

namespace trlc::enums::policies {

/**
 * @brief CRTP base class for comparison policies
 *
 * Enforces the required interface for all comparison policies:
 * - static constexpr int compare(T, T)
 * - static constexpr bool less(T, T)
 * - static constexpr bool equal(T, T)
 * - static constexpr bool greater(T, T)
 *
 */
template <typename Derived, typename T>
struct ComparisonPolicyBase {
    using value_type = T;

    /**
     * @brief Compare two values
     * @return <0 if lhs < rhs, 0 if lhs == rhs, >0 if lhs > rhs
     */
    static constexpr int compare(T lhs, T rhs) { return Derived::compare(lhs, rhs); }

    /**
     * @brief Check if lhs is less than rhs
     */
    static constexpr bool less(T lhs, T rhs) { return Derived::compare(lhs, rhs) < 0; }

    /**
     * @brief Check if lhs equals rhs
     */
    static constexpr bool equal(T lhs, T rhs) { return Derived::compare(lhs, rhs) == 0; }

    /**
     * @brief Check if lhs is greater than rhs
     */
    static constexpr bool greater(T lhs, T rhs) { return Derived::compare(lhs, rhs) > 0; }

protected:
    // Prevent direct instantiation
    ComparisonPolicyBase() = default;
    ~ComparisonPolicyBase() = default;
    ComparisonPolicyBase(const ComparisonPolicyBase&) = default;
    ComparisonPolicyBase(ComparisonPolicyBase&&) = default;
    ComparisonPolicyBase& operator=(const ComparisonPolicyBase&) = default;
    ComparisonPolicyBase& operator=(ComparisonPolicyBase&&) = default;
};

// ============================================================================
// LexicographicComparisonPolicy - Standard comparison
// ============================================================================

/**
 * @brief Lexicographic comparison policy using operator< and operator==
 *
 * Standard comparison using built-in operators.
 * Works with any type that supports operator< and operator==.
 *
 * Use cases:
 * - Default comparison for arithmetic types (int, double, etc.)
 * - String comparison (std::string_view)
 * - Any custom type with operator< and operator==
 *
 * Behavior:
 * - Returns negative if lhs < rhs
 * - Returns 0 if lhs == rhs
 * - Returns positive if lhs > rhs
 *
 * Requirements:
 * - T must be comparable with operator<
 * - T must be comparable with operator==
 *
 * @tparam T Value type (must support operator< and operator==)
 */
template <typename T, bool isCaseSensitive>
struct LexicographicComparisonPolicy
    : ComparisonPolicyBase<LexicographicComparisonPolicy<T, isCaseSensitive>, T> {
    /**
     * @brief Compare two values using standard operators
     *
     * @param lhs Left-hand side value
     * @param rhs Right-hand side value
     * @return <0 if lhs < rhs, 0 if lhs == rhs, >0 if lhs > rhs
     */
    template <typename U = T>
    static constexpr auto compare(T lhs, T rhs)
        -> std::enable_if_t<!std::is_same_v<U, std::string_view>, int> {
        if (lhs < rhs) {
            return -1;
        }
        if (lhs == rhs) {
            return 0;
        }
        return 1;
    }

    template <typename U = T>
    static constexpr auto compare(T lhs, T rhs)
        -> std::enable_if_t<std::is_same_v<U, std::string_view>, int> {
        if constexpr (isCaseSensitive) {
            // Use string_view's built-in comparison
            const int result = lhs.compare(rhs);
            // Normalize to -1, 0, 1
            return (result > 0 ? 1 : 0) - (result < 0 ? 1 : 0);

        } else {
            const std::size_t min_len = (lhs.size() < rhs.size()) ? lhs.size() : rhs.size();

            // Compare character by character (case-insensitive)
            for (std::size_t i = 0; i < min_len; ++i) {
                const char lhs_lower = to_lower(lhs[i]);
                const char rhs_lower = to_lower(rhs[i]);

                if (lhs_lower < rhs_lower) {
                    return -1;
                }
                if (lhs_lower > rhs_lower) {
                    return 1;
                }
            }

            // All compared characters are equal, check length
            if (lhs.size() < rhs.size()) {
                return -1;
            }
            if (lhs.size() > rhs.size()) {
                return 1;
            }
            return 0;
        }
    }

private:
    /**
     * @brief Convert ASCII character to lowercase
     * @param c Character to convert
     * @return Lowercase version if c is A-Z, otherwise c unchanged
     */
    static constexpr char to_lower(char c) {
        return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
    }
};

}  // namespace trlc::enums::policies
