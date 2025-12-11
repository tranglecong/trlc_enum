#pragma once

#include <cstddef>
#include <iterator>

namespace trlc::enums {

/**
 * @brief Const iterator for enum values
 *
 * Provides a random access const iterator interface that works at compile-time.
 * All operations are constexpr-compatible.
 *
 * @tparam Enum The enum type (must be trlc::enums::Enum<Manager>)
 */
template <typename Enum>
struct Iterator {
    // ========================================================================
    // Iterator Traits (required by std::iterator_traits)
    // ========================================================================
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Enum;
    using difference_type = std::ptrdiff_t;
    using pointer = const Enum*;
    using reference = Enum;  // Return by value (const iterator)

    // ========================================================================
    // Constructors
    // ========================================================================

    /**
     * @brief Default constructor - points to index 0
     */
    constexpr Iterator() = default;

    /**
     * @brief Construct iterator at given index
     * @param idx Index position
     */
    constexpr explicit Iterator(std::size_t idx) : _index(idx) {}

    constexpr Iterator(const Iterator&) = default;
    constexpr Iterator(Iterator&&) = default;
    constexpr Iterator& operator=(const Iterator&) = default;
    constexpr Iterator& operator=(Iterator&&) = default;
    ~Iterator() = default;

    // ========================================================================
    // Dereference Operators
    // ========================================================================

    /**
     * @brief Dereference operator - return enum at current index
     * @return Enum value at current index
     */
    constexpr reference operator*() const { return Enum{Enum::toSortedIndex(_index)}; }

    /**
     * @brief Arrow operator - access members of element
     * @return Pointer to enum (for accessing members)
     */
    constexpr pointer operator->() const {
        // Reconstruct temp in-place since Enum has no copy assignment
        _temp.~Enum();
        new (&_temp) Enum{Enum::toSortedIndex(_index)};
        return &_temp;
    }

    /**
     * @brief Subscript operator - access element at offset
     * @param n Offset from current position
     * @return Enum at position (current + n)
     */
    constexpr reference operator[](difference_type n) const {
        return Enum{Enum::toSortedIndex(_index + static_cast<std::size_t>(n))};
    }

    // ========================================================================
    // Increment/Decrement Operators
    // ========================================================================

    /**
     * @brief Pre-increment operator (++it)
     * @return Reference to this iterator after increment
     */
    constexpr Iterator& operator++() {
        ++_index;
        return *this;
    }

    /**
     * @brief Post-increment operator (it++)
     * @return Copy of iterator before increment
     */
    constexpr const Iterator operator++(int) {  // NOLINT
        Iterator tmp = *this;
        ++_index;
        return tmp;
    }

    /**
     * @brief Pre-decrement operator (--it)
     * @return Reference to this iterator after decrement
     */
    constexpr Iterator& operator--() {
        --_index;
        return *this;
    }

    /**
     * @brief Post-decrement operator (it--)
     * @return Copy of iterator before decrement
     */
    constexpr const Iterator operator--(int) {  // NOLINT
        Iterator tmp = *this;
        --_index;
        return tmp;
    }

    // ========================================================================
    // Arithmetic Operators
    // ========================================================================

    /**
     * @brief Addition assignment (it += n)
     * @param n Number of positions to advance
     * @return Reference to this iterator
     */
    constexpr Iterator& operator+=(difference_type n) {
        _index = static_cast<std::size_t>(static_cast<difference_type>(_index) + n);
        return *this;
    }

    /**
     * @brief Subtraction assignment (it -= n)
     * @param n Number of positions to move back
     * @return Reference to this iterator
     */
    constexpr Iterator& operator-=(difference_type n) {
        _index = static_cast<std::size_t>(static_cast<difference_type>(_index) - n);
        return *this;
    }

    /**
     * @brief Addition operator (it + n)
     * @param n Number of positions to advance
     * @return New iterator at position (current + n)
     */
    constexpr Iterator operator+(difference_type n) const {
        return Iterator(_index + static_cast<std::size_t>(n));
    }

    /**
     * @brief Subtraction operator (it - n)
     * @param n Number of positions to move back
     * @return New iterator at position (current - n)
     */
    constexpr Iterator operator-(difference_type n) const {
        return Iterator(_index - static_cast<std::size_t>(n));
    }

    /**
     * @brief Distance between two iterators (it1 - it2)
     * @param other Other iterator
     * @return Distance between iterators
     */
    constexpr difference_type operator-(const Iterator& other) const {
        return static_cast<difference_type>(_index) - static_cast<difference_type>(other._index);
    }

    // ========================================================================
    // Comparison Operators
    // ========================================================================

    /**
     * @brief Equality comparison
     * @param other Other iterator
     * @return true if both point to same index
     */
    constexpr bool operator==(const Iterator& other) const { return _index == other._index; }

    /**
     * @brief Inequality comparison
     * @param other Other iterator
     * @return true if iterators point to different indices
     */
    constexpr bool operator!=(const Iterator& other) const { return _index != other._index; }

    /**
     * @brief Less-than comparison
     * @param other Other iterator
     * @return true if this iterator is before other
     */
    constexpr bool operator<(const Iterator& other) const { return _index < other._index; }

    /**
     * @brief Greater-than comparison
     * @param other Other iterator
     * @return true if this iterator is after other
     */
    constexpr bool operator>(const Iterator& other) const { return _index > other._index; }

    /**
     * @brief Less-than-or-equal comparison
     * @param other Other iterator
     * @return true if this iterator is before or at same position as other
     */
    constexpr bool operator<=(const Iterator& other) const { return _index <= other._index; }

    /**
     * @brief Greater-than-or-equal comparison
     * @param other Other iterator
     * @return true if this iterator is after or at same position as other
     */
    constexpr bool operator>=(const Iterator& other) const { return _index >= other._index; }

    // ========================================================================
    // Accessor Methods
    // ========================================================================

    /**
     * @brief Get current index
     * @return Current index position
     */
    constexpr std::size_t index() const { return _index; }

private:
    std::size_t _index{0};
    mutable Enum _temp{Enum::toSortedIndex(0)};  // Temporary for operator->
};

// ============================================================================
// Non-member Operators (for it + n where n comes first: n + it)
// ============================================================================

/**
 * @brief Addition operator (n + it)
 * @param n Number of positions to advance
 * @param it Iterator
 * @return New iterator at position (it + n)
 */
template <typename Enum>
constexpr Iterator<Enum> operator+(typename Iterator<Enum>::difference_type n,
                                   const Iterator<Enum>& it) {
    return it + n;
}

}  // namespace trlc::enums