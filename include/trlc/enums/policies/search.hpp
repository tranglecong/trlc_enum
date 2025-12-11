#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <string_view>
#include <type_traits>

namespace trlc::enums::policies {

// ============================================================================
// Helper Functions for Case-Insensitive Comparison
// ============================================================================

namespace detail {

/**
 * @brief Convert character to lowercase (constexpr)
 */
constexpr char toLower(char c) {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
}

/**
 * @brief Case-insensitive comparison for string_view
 */
constexpr bool equalCaseInsensitive(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (toLower(a[i]) != toLower(b[i])) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Case-insensitive less-than comparison for string_view
 */
constexpr bool lessCaseInsensitive(std::string_view a, std::string_view b) {
    std::size_t min_size = (a.size() < b.size()) ? a.size() : b.size();
    for (std::size_t i = 0; i < min_size; ++i) {
        char lower_a = toLower(a[i]);
        char lower_b = toLower(b[i]);
        if (lower_a != lower_b) {
            return lower_a < lower_b;
        }
    }
    return a.size() < b.size();
}

/**
 * @brief Generic equality comparison with case sensitivity control
 */
template <typename T, bool isCaseSensitive, typename U>
constexpr bool areEqual(T a, U b) {
    // Convert to string_view if comparing strings
    if constexpr (std::is_same_v<T, std::string_view> ||
                  std::is_convertible_v<U, std::string_view>) {
        std::string_view sv_a = a;
        std::string_view sv_b = b;
        if constexpr (!isCaseSensitive) {
            return equalCaseInsensitive(sv_a, sv_b);
        } else {
            return sv_a == sv_b;
        }
    } else {
        return a == b;
    }
}

/**
 * @brief Generic less-than comparison with case sensitivity control
 */
template <typename T, bool isCaseSensitive>
constexpr bool isLess(T a, T b) {
    if constexpr (std::is_same_v<T, std::string_view> && !isCaseSensitive) {
        return lessCaseInsensitive(a, b);
    } else {
        return a < b;
    }
}

}  // namespace detail

template <typename Derived, typename T>
struct SearchPolicyBase {
    // Interface enforcement via CRTP
    template <typename EnumData, std::size_t N>
    static constexpr std::optional<std::size_t> searchByValue(const std::array<EnumData, N>& datas,
                                                              T target) {
        auto valueAccessor = [](const EnumData& data) { return data.value; };
        return Derived::search(datas, target, valueAccessor);
    }

    template <typename EnumData, std::size_t N>
    static constexpr std::optional<std::size_t> searchByName(const std::array<EnumData, N>& datas,
                                                             T target) {
        auto nameAccessor = [](const EnumData& data) { return data.name; };
        return Derived::search(datas, target, nameAccessor);
    }

protected:
    // Prevent direct instantiation
    SearchPolicyBase() = default;
    ~SearchPolicyBase() = default;
    SearchPolicyBase(const SearchPolicyBase&) = default;
    SearchPolicyBase(SearchPolicyBase&&) = default;
    SearchPolicyBase& operator=(const SearchPolicyBase&) = default;
    SearchPolicyBase& operator=(SearchPolicyBase&&) = default;
};

template <typename T, bool isCaseSensitive = true>
struct LinearSearchPolicy : SearchPolicyBase<LinearSearchPolicy<T, isCaseSensitive>, T> {
    template <typename EnumData, std::size_t N, typename Accessor>
    static constexpr std::optional<std::size_t> search(const std::array<EnumData, N>& datas,
                                                       T target,
                                                       Accessor accessor) {
        for (std::size_t i = 0; i < N; ++i) {
            if (detail::areEqual<T, isCaseSensitive>(accessor(datas[i]), target)) {
                return i;
            }
        }
        return std::nullopt;
    }
};

template <typename T, bool isCaseSensitive = true>
struct BinarySearchPolicy : SearchPolicyBase<BinarySearchPolicy<T, isCaseSensitive>, T> {
    template <typename EnumData, std::size_t N, typename Accessor>
    static constexpr std::optional<std::size_t> search(const std::array<EnumData, N>& datas,
                                                       T target,
                                                       Accessor accessor) {
        if constexpr (N == 0) {
            return std::nullopt;
        }

        std::size_t left = 0;
        std::size_t right = N;

        while (left < right) {
            std::size_t mid = left + (right - left) / 2;

            if (detail::areEqual<T, isCaseSensitive>(accessor(datas[mid]), target)) {
                return mid;
            }
            if (detail::isLess<T, isCaseSensitive>(accessor(datas[mid]), target)) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }

        return std::nullopt;
    }
};

}  // namespace trlc::enums::policies
