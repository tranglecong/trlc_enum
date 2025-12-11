#pragma once

#include <cstddef>
#include <string_view>
#include <type_traits>

#include "trlc/macro/index_strings.hpp"

namespace trlc::enums::policies {

// ============================================================================
// CRTP Base Class for IncrementPolicy
// ============================================================================

/**
 * @brief CRTP base class for increment policies
 *
 * Enforces the required interface for all increment policies:
 * - static constexpr T increment(T current, std::size_t index)
 * - static constexpr T startValue()
 *
 * Usage:
 * @code
 * template <typename T>
 * struct MyIncrementPolicy : IncrementPolicyBase<MyIncrementPolicy<T>, T> {
 *     static constexpr T increment(T current, std::size_t index) { ... }
 *     static constexpr T startValue() { ... }
 * };
 * @endcode
 *
 * @tparam Derived The derived policy class (CRTP pattern)
 * @tparam T The value type this policy operates on
 */
template <typename Derived, typename T>
struct IncrementPolicyBase {
    using value_type = T;

    // Interface enforcement via CRTP
    static constexpr T increment(T current, std::size_t index) {
        return Derived::increment(current, index);
    }

    static constexpr T startValue() { return Derived::startValue(); }

protected:
    // Prevent direct instantiation
    IncrementPolicyBase() = default;
    ~IncrementPolicyBase() = default;
    IncrementPolicyBase(const IncrementPolicyBase&) = default;
    IncrementPolicyBase(IncrementPolicyBase&&) = default;
    IncrementPolicyBase& operator=(const IncrementPolicyBase&) = default;
    IncrementPolicyBase& operator=(IncrementPolicyBase&&) = default;
};

// ============================================================================
// Default IncrementPolicy implementations
// ============================================================================

/**
 * @brief Default increment policy trait for enum value auto-filling
 *
 * Provides compile-time increment behavior for different types:
 * - Arithmetic types (int, float, double, etc.): increment by +1
 * - string_view: use auto-generated compile-time index strings
 *
 * Users can specialize this trait for custom types or behaviors.
 *
 * Example custom policy:
 * @code
 * template <>
 * struct DefaultIncrementPolicy<CustomType> {
 *     static constexpr CustomType increment(CustomType current, std::size_t index) {
 *         return current.next();
 *     }
 *     static constexpr CustomType startValue() { return CustomType::first(); }
 * };
 * @endcode
 *
 * @tparam T Value type
 * @tparam Enable SFINAE enable condition
 */
template <typename T, typename Enable = void>
struct DefaultIncrementPolicy;

/**
 * @brief Increment policy for arithmetic types (int, float, double, etc.)
 *
 * Provides simple +1 increment for all arithmetic types.
 * Start value is 0 (or 0.0 for floating-point types).
 *
 * Examples:
 *   - int: 0 → 1 → 2 → 3 → ...
 *   - float: 0.0 → 1.0 → 2.0 → 3.0 → ...
 *   - double: 0.0 → 1.0 → 2.0 → 3.0 → ...
 *
 * @tparam T Arithmetic type (int, float, double, etc.)
 */
template <typename T>
struct DefaultIncrementPolicy<T, std::enable_if_t<std::is_arithmetic_v<T>>>
    : IncrementPolicyBase<DefaultIncrementPolicy<T, std::enable_if_t<std::is_arithmetic_v<T>>>, T> {
    /**
     * @brief Increment value by 1
     * @param current Current value
     * @param index Current fill index (unused for arithmetic types)
     * @return current + 1
     */
    static constexpr T increment(T current, std::size_t /* index */) {
        return current + static_cast<T>(1);
    }

    /**
     * @brief Get starting value (0 for arithmetic types)
     * @return 0 (or 0.0 for floating-point)
     */
    static constexpr T startValue() { return static_cast<T>(0); }
};

/**
 * @brief Increment policy for std::string_view
 *
 * Uses compile-time generated index strings: "0", "1", "2", ...
 * Leverages the global trlcConstexprIndexString() function for O(1) lookup.
 *
 * Examples:
 *   - Index 0: "" → "0"
 *   - Index 1: "" → "1"
 *   - Index 2: "" → "2"
 *
 * Note: The index parameter is used to determine which string to return.
 * This ensures unique string values for each enum element.
 */
template <>
struct DefaultIncrementPolicy<std::string_view>
    : IncrementPolicyBase<DefaultIncrementPolicy<std::string_view>, std::string_view> {
    /**
     * @brief Get next index string
     * @param current Current string_view (unused)
     * @param index Current fill index
     * @return string_view for (index + 1), e.g., "0", "1", "2", ...
     */
    static constexpr std::string_view increment(std::string_view /* current */, std::size_t index) {
        // index is the current fill count, return the next index string
        return ::trlcConstexprIndexString(index + 1);
    }

    /**
     * @brief Get starting value ("0" for string_view)
     * @return "0"
     */
    static constexpr std::string_view startValue() { return ::trlcConstexprIndexString(0); }
};

}  // namespace trlc::enums::policies
