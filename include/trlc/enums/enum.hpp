#pragma once

#include <cstddef>
#include <type_traits>

namespace trlc::enums {

// Core enum instance type that holds an _index into the Manager's arrays
// Manager provides: names[], values[], descs[], extras[], Policies
// Usage: Color::Enum car = Color::SEDAN;
template <typename Manager>
struct Enum {
    // Type aliases from Manager's policies
    using Policies = typename Manager::Policies;
    using NameType = typename Manager::NameType;
    using ValueType = typename Manager::ValueType;
    using DescType = typename Manager::DescType;
    using ExtraType = typename Manager::ExtraType;

    // Default constructor: _index = 0 (first enum constant)
    constexpr Enum() : _index{0} {};

    // Construct from index (used by enum constants)
    constexpr explicit Enum(std::size_t idx) : _index(idx) {}

    // Copy/move operations (enabled because _index is non-const)
    constexpr Enum(const Enum&) = default;
    Enum(Enum&&) = default;
    Enum& operator=(const Enum&) = default;
    Enum& operator=(Enum&&) = default;
    ~Enum() = default;

    // Accessor methods: lookup Manager's arrays using _index
    constexpr NameType name() const { return Manager::datas[_index].name; }

    constexpr ValueType value() const { return Manager::datas[_index].value; }

    constexpr DescType desc() const { return Manager::datas[_index].desc; }

    constexpr ExtraType extra() const { return Manager::datas[_index].extra; }

    [[nodiscard]] constexpr std::size_t index() const { return _index; }

    constexpr auto enumName() const { return Manager::NAME; }

    // Comparison operators: use Policies::Comparison on values
    template <typename Other>
    constexpr auto operator==(const Other& other) const
        -> std::enable_if_t<std::is_same_v<Enum, Other>, bool> {
        return Policies::Comparison::equal(value(), other.value());
    }

    template <typename Other>
    constexpr auto operator<(const Other& other) const
        -> std::enable_if_t<std::is_same_v<Enum, Other>, bool> {
        return Policies::Comparison::less(value(), other.value());
    }

    template <typename Other>
    constexpr auto operator>(const Other& other) const
        -> std::enable_if_t<std::is_same_v<Enum, Other>, bool> {
        return Policies::Comparison::greater(value(), other.value());
    }

    template <typename Other>
    constexpr auto operator!=(const Other& other) const
        -> std::enable_if_t<std::is_same_v<Enum, Other>, bool> {
        return !Policies::Comparison::equal(value(), other.value());
    }

    [[nodiscard]] static constexpr std::size_t toSortedIndex(std::size_t idx) {
        return Manager::sorted_indices[idx];
    }

private:
    // Index into Manager's arrays
    std::size_t _index;

};  // struct Enum

}  // namespace trlc::enums