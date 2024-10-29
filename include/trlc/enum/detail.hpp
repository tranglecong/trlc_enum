#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string_view>

namespace trlc
{

// clang-format off
#define TRLC_ENUM_NORMALIZE_HELPER() trlc::enum_feild::NormalizeHelper()

#define TRLC_FIELD(...) TRLC_ENUM_NORMALIZE_HELPER() (TRLC_APPEND(TRLC_STRINGIFY, __delim__, __VA_ARGS__)) * int64_t

#define TRLC_ENUM_DECLARE_HELPER(varname, index) static constexpr enum_type varname{index - 1}

#define TRLC_ENUM_DETAIL(enumname, enumdef, ...)                                                                                                              \
    struct enumname                                                                                                                                           \
    {                                                                                                                                                         \
        using enum_def = enumdef<enumname>;                                                                                                                   \
        using enum_type = enum_def::enum_type;                                                                                                                \
        using value_type = enum_def::value_type;                                                                                                              \
        using name_type = enum_def::enum_type::name_type;                                                                                                     \
        using iterator_type = enum_def::iterator;                                                                                                             \
        inline static constexpr std::string_view m_tag{#enumname};                                                                                            \
        inline static constexpr auto m_values = trlc::enum_feild::array_values({TRLC_EXPAND(TRLC_STRINGIFY, __VA_ARGS__)});                                   \
        inline static constexpr auto m_names = trlc::enum_feild::create_array_name({TRLC_EXPAND(TRLC_STRINGIFY, __VA_ARGS__)});                               \
        inline static constexpr auto m_descs = trlc::enum_feild::create_array_description({TRLC_EXPAND(TRLC_STRINGIFY, __VA_ARGS__)});                        \
        inline static constexpr auto m_size = m_values.size();                                                                                                \
        inline static constexpr auto m_array = trlc::enum_feild::create_array_enum<enumname>();                                                               \
        TRLC_APPLY_WITH_INDEX(TRLC_ENUM_DECLARE_HELPER, __VA_ARGS__)                                                                                          \
        static constexpr trlc::EnumHelper<enum_def::holder, enum_def::value_search_policy, enum_def::name_search_policy, enum_def ::unknown_policy> helper{}; \
        inline static constexpr iterator_type iterator{};                                                                                                     \
        static constexpr std::optional<enum_type> fromValue(value_type value)                                                                                 \
        {                                                                                                                                                     \
            return helper.fromValue(value);                                                                                                                   \
        }                                                                                                                                                     \
        static constexpr std::optional<enum_type> fromString(const name_type& name)                                                                           \
        {                                                                                                                                                     \
            return helper.fromString(name);                                                                                                                   \
        }                                                                                                                                                     \
        static constexpr auto size()                                                                                                                          \
        {                                                                                                                                                     \
            return m_size;                                                                                                                                    \
        }                                                                                                                                                     \
        static constexpr auto tag()                                                                                                                           \
        {                                                                                                                                                     \
            return m_tag;                                                                                                                                     \
        }                                                                                                                                                     \
        static const std::string dump()                                                                                                                       \
        {                                                                                                                                                     \
            std::string result{};                                                                                                                             \
            result += "[\n";                                                                                                                                  \
            for (auto item : iterator)                                                                                                                        \
            {                                                                                                                                                 \
                result += " ";                                                                                                                                \
                result += item.dump();                                                                                                                        \
                result += ",\n";                                                                                                                              \
            }                                                                                                                                                 \
            result += "]";                                                                                                                                    \
            return result;                                                                                                                                    \
        }                                                                                                                                                     \
    };
// clang-format on

/**
 * @brief A generic enum class to hold enum value, name, and description.
 *
 * @tparam T The type of the enum value.
 * @tparam Holder A class that provides access to arrays of enum values, names, and descriptions.
 */
template<typename T, class Holder>
struct Enum
{
    using value_type = T;               ///< The type of the enum value.
    using name_type = std::string_view; ///< The type for the enum name.
    using desc_type = std::string_view; ///< The type for the enum description.

    T m_value{};               ///< The enum value.
    std::string_view m_name{}; ///< The name of the enum.
    std::string_view m_desc{}; ///< The description of the enum.

    constexpr Enum() = default;

    /**
     * @brief Constructs an Enum by its index.
     *
     * @param index The index of the enum in the Holder.
     */
    constexpr Enum(size_t index)
        : m_value{Holder::m_values[index]}
        , m_name{Holder::m_names[index]}
        , m_desc{Holder::m_descs[index]}
    {
    }

    /**
     * @brief Constructs an Enum with specified value, name, and description.
     *
     * @param value The enum value.
     * @param name The name of the enum.
     * @param desc The description of the enum.
     */
    constexpr Enum(value_type value, const name_type& name, const desc_type& desc)
        : m_value{value}
        , m_name{name}
        , m_desc{desc}
    {
    }

    /**
     * @brief Returns the value of the enum.
     *
     * @return The enum value.
     */
    constexpr T value() const
    {
        return m_value;
    }

    /**
     * @brief Returns the name of the enum.
     *
     * @return The name of the enum.
     */
    constexpr std::string_view name() const
    {
        return m_name;
    }

    /**
     * @brief Returns the description of the enum.
     *
     * @return The description of the enum.
     */
    constexpr std::string_view desc() const
    {
        return m_desc;
    }

    /**
     * @brief Compares two Enum objects for equality.
     *
     * @param other The other Enum object to compare with.
     * @return True if both Enum objects have the same value; otherwise, false.
     */
    constexpr bool operator==(const Enum<T, Holder>& other) const
    {
        if (m_value == other.m_value)
        {
            return true;
        }
        return false;
    }

    /**
     * @brief Compares two Enum objects for inequality.
     *
     * @param other The other Enum object to compare with.
     * @return True if the Enum objects have different values; otherwise, false.
     */
    constexpr bool operator!=(const Enum<T, Holder>& other) const
    {
        if (m_value != other.m_value)
        {
            return true;
        }
        return false;
    }

    /**
     * @brief Conversion operator to the underlying type.
     *
     * @return The enum value.
     */
    constexpr operator T() const
    {
        return m_value;
    }

    /**
     * @brief Returns the holder instance.
     *
     * @return A Holder instance.
     */
    constexpr Holder holder() const
    {
        return Holder{};
    }

    /**
     * @brief Retrieves the tag associated with the Enum's Holder.
     *
     * @return The tag of the Holder.
     */
    constexpr auto tag() const
    {
        return Holder::tag();
    }

    /**
     * @brief Returns a JSON-like string representation of the Enum.
     *
     * @return A string dump of the enum's data.
     */
    const std::string dump() const
    {
        using namespace std::literals;
        return "{\"tag\": \""s + std::string{tag()} + ", \"value\": "s + std::to_string(static_cast<int>(m_value)) + ", \"name\": \""s + std::string{name()} + "\", \"desc\": \""s + std::string{desc()} + "\"}"s;
    }
};

/**
 * @brief Helper class providing search functionality for enums.
 *
 * @tparam Holder A class that stores enum information.
 * @tparam EnumSearchPolicy Policy used for searching enums by value.
 * @tparam StringSearchPolicy Policy for searching enums by string.
 * @tparam UnknownPolicy Policy for handling unknown enum values.
 */
template<typename Holder, class EnumSearchPolicy, class StringSearchPolicy, class UnknownPolicy>
struct EnumHelper
{
    /**
     * @brief Searches for an enum by its value.
     *
     * @param value The enum value to search for.
     * @return An std::optional containing the found enum or nullopt if not found.
     */
    constexpr std::optional<typename Holder::enum_type> fromValue(typename Holder::value_type value) const
    {
        std::optional<size_t> result{EnumSearchPolicy::template search<Holder>(value)};
        if (result)
        {
            auto index{result.value()};
            return Holder::m_array[index];
        }
        return UnknownPolicy::template handle<Holder>();
    }

    /**
     * @brief Searches for an enum by its name.
     *
     * @param name The name of the enum to search for.
     * @return An std::optional containing the found enum or nullopt if not found.
     */
    constexpr std::optional<typename Holder::enum_type> fromString(std::string_view name) const
    {
        std::optional<size_t> result{StringSearchPolicy::template search<Holder>(name)};
        if (result)
        {
            auto index{result.value()};
            return Holder::m_array[index];
        }
        return UnknownPolicy::template handle<Holder>();
    }
};

/**
 * @brief Iterator class for Enum.
 *
 * @tparam Holder A class that holds the enum data.
 */
template<class Holder>
struct EnumIterator
{
    struct Iterator
    {
        std::size_t index{};

        constexpr Iterator(std::size_t idx)
            : index{idx}
        {
        }

        // Dereference operator
        constexpr const typename Holder::enum_type operator*() const
        {
            return Holder::m_array[index];
        }

        // Pre-increment operator
        constexpr Iterator& operator++()
        {
            ++index;
            return *this;
        }

        // Comparison operators
        constexpr bool operator!=(const Iterator& other) const
        {
            return index != other.index;
        }
        constexpr bool operator==(const Iterator& other) const
        {
            return index == other.index;
        }
    };

    /**
     * @brief Returns an iterator to the beginning of the enum array.
     *
     * @return Iterator pointing to the start of the array.
     */
    constexpr Iterator begin() const
    {
        constexpr size_t START_INDEX{0};
        return Iterator(START_INDEX);
    }

    /**
     * @brief Returns an iterator to the end of the enum array.
     *
     * @return Iterator pointing past the last element.
     */
    constexpr Iterator end() const
    {
        return Iterator(Holder::size());
    }
};

namespace policy
{

/**
 * @brief Policy for linear searching of enum values.
 */
struct LinearSearchPolicy
{
    /**
     * @brief Performs a linear search for an enum value.
     *
     * @tparam Holder The holder class storing the enum values.
     * @param value The enum value to search for.
     * @return An std::optional containing the index of the found enum or nullopt if not found.
     */
    template<class Holder>
    static constexpr std::optional<size_t> search(typename Holder::value_type value)
    {
        std::optional<size_t> found_index{};
        for (size_t index{0}; index < Holder::m_values.size(); ++index)
        {

            if (Holder::m_values[index] == value)
            {
                found_index = index;
                break;
            }
        }
        return found_index;
    }
};

/**
 * @brief Policy for binary searching of enum values.
 */
struct BinarySearchPolicy
{
    /**
     * @brief Performs a binary search for an enum value.
     *
     * @tparam Holder The holder class storing the enum values.
     * @param value The enum value to search for.
     * @return An std::optional containing the index of the found enum or nullopt if not found.
     */
    template<class Holder>
    static constexpr std::optional<size_t> search(typename Holder::value_type value)
    {
        std::optional<size_t> found_index{};
        // Perform binary search
        size_t left = 0;
        size_t right = Holder::m_array.size() - 1;

        while (left <= right)
        {
            size_t mid = left + (right - left) / 2;

            if (Holder::m_array[mid].value() == value)
            {
                found_index = mid;
                break;
            }
            if (Holder::m_array[mid].value() < value)
            {
                left = mid + 1;
            }
            else
            {
                right = mid - 1;
            }
        }
        return found_index;
    }
};

/**
 * @brief Policy for case-sensitive string searching of enum names.
 */
struct CaseSensitiveStringSearchPolicy
{
    /**
     * @brief Performs a case-sensitive search for an enum name.
     *
     * @tparam Holder The holder class storing the enum names.
     * @param name The name to search for.
     * @return An std::optional containing the index of the found enum or nullopt if not found.
     */
    template<class Holder>
    static constexpr std::optional<size_t> search(const typename Holder::name_type& name)
    {
        std::optional<size_t> found_index{};
        for (size_t index{0}; index < Holder::m_names.size(); ++index)
        {

            if (Holder::m_names[index] == name)
            {
                found_index = index;
                break;
            }
        }

        return found_index;
    }
};

/**
 * @brief Policy for case-insensitive string searching of enum names.
 */
struct CaseInsensitiveStringSearchPolicy
{
    /**
     * @brief Compares two characters case-insensitively.
     *
     * @param a The first character.
     * @param b The second character.
     * @return True if the characters are equal ignoring case; otherwise false.
     */
    static constexpr bool caseInsensitiveEqual(char a, char b)
    {
#if __cplusplus >= 202002L
// C++20 or later: use std::tolower as it is constexpr
#include <cctype>
        return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
#else
        return toLower(static_cast<unsigned char>(a)) == toLower(static_cast<unsigned char>(b));
#endif
    }

    /**
     * @brief Performs a case-insensitive search for an enum name.
     *
     * @tparam Holder The holder class storing the enum names.
     * @param name The name to search for.
     * @return An std::optional containing the index of the found enum or nullopt if not found.
     */
    template<class Holder>
    static constexpr std::optional<size_t> search(const typename Holder::name_type& name)
    {
        std::optional<size_t> found_index{};
        for (size_t index{0}; index < Holder::m_names.size(); ++index)
        {

            if (name.size() == Holder::m_names[index].size() && std::equal(Holder::m_names[index].begin(), Holder::m_names[index].end(), name.begin(), caseInsensitiveEqual))
            {
                found_index = index;
                break;
            }
        }

        return found_index;
    }

    /**
     * @brief Converts a single character to lowercase for case-insensitive comparison.
     *
     * @param ch The character to convert to lowercase.
     * @return The lowercase equivalent if the input is an uppercase letter; otherwise, returns the character unchanged.
     */
    static constexpr char toLower(char ch)
    {
        return (ch >= 'A' && ch <= 'Z') ? ch + ('a' - 'A') : ch;
    }
};

/**
 * @brief Policy for handling unknown enum values.
 */
struct UnknownPolicy
{
    /**
     * @brief Handles unknown enums by returning nullopt.
     *
     * @tparam Holder The holder class storing the enum data.
     * @return An std::optional containing a nullopt to signify no match.
     */
    template<class Holder>
    static constexpr std::optional<typename Holder::enum_type> handle()
    {
        return std::nullopt;
    }
};
} // namespace policy

} // namespace trlc