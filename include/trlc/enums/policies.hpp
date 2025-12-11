#pragma once

#include "trlc/enums/policies/comparison.hpp"
#include "trlc/enums/policies/increment.hpp"
#include "trlc/enums/policies/search.hpp"

namespace trlc::enums::policies {

// DefaultPolicies: Standard configuration for TRLC_ENUM
//
// Template parameters customize enum behavior:
//   Value: Type of enum values (default: int)
//   Desc: Type of descriptions (default: std::string_view)
//   Extra: Type of extra data (default: std::string_view)
//   CaseSensitive: Name/value comparison mode (default: true)
//
// Behaviors:
//   - Increment: Missing values auto-filled with +1
//   - Search: Binary search O(log n) on sorted value indices
//   - Comparison: Lexicographic ordering by value
//   - Case sensitivity: Configurable for names and string values
//
// Example with custom types:
//   TRLC_ENUM_DEF(MyEnum, DefaultPolicies<long, const char*, int, false>, ...)
//   // ValueType=long, DescType=const char*, ExtraType=int, case-insensitive

template <typename Value = int,
          typename Desc = std::string_view,
          typename Extra = std::string_view,
          bool CaseSensitive = true>
struct DefaultPolicies {
    using ValueType = Value;    // Type for enum values
    using DescType = Desc;      // Type for descriptions
    using ExtraType = Extra;    // Type for extra metadata

    inline static constexpr bool CASE_SENSITIVE{CaseSensitive};

    using Increment = DefaultIncrementPolicy<ValueType>;
    using Search = BinarySearchPolicy<ValueType, CASE_SENSITIVE>;
    using Comparison = LexicographicComparisonPolicy<ValueType, CASE_SENSITIVE>;
};

// ============================================================================
// EXAMPLE 1: String values with case-insensitive search
// ============================================================================
//
// Use case: Enum with string values (e.g., commands, color names)
// Features: Case-insensitive lookup, linear search for string values
//
// struct StringValuePolicies {
//     using ValueType = std::string_view;
//     using DescType = std::string_view;
//     using ExtraType = std::string_view;
//     inline static constexpr bool CASE_SENSITIVE{false};
//
//     using Increment = DefaultIncrementPolicy<ValueType>;
//     using Search = LinearSearchPolicy<ValueType, CASE_SENSITIVE>;
//     using Comparison = LexicographicComparisonPolicy<ValueType, CASE_SENSITIVE>;
// };
//
// TRLC_ENUM_DEF(Command, StringValuePolicies,
//     Help = TRLC_FIELD(value = "help", desc = "Show help"),
//     Exit = TRLC_FIELD(value = "exit", desc = "Exit program")
// );
//
// Command::fromValue("HELP");  // Works (case-insensitive)
// Command::fromName("help");   // Works (case-insensitive)

// ============================================================================
// EXAMPLE 2: Powers of 2 for bit flags
// ============================================================================
//
// Use case: Bit flags for permissions, options, etc.
// Features: Auto-increment as powers of 2 (1, 2, 4, 8, ...)
//
// struct PowerOf2Increment {
//     static constexpr int startValue() { return 1; }
//     static constexpr int increment(int current, std::size_t /* index */) {
//         return current * 2;
//     }
// };
//
// struct PowerOf2Policies {
//     using ValueType = int;
//     using DescType = std::string_view;
//     using ExtraType = std::string_view;
//     inline static constexpr bool CASE_SENSITIVE{true};
//
//     using Increment = PowerOf2Increment;
//     using Search = LinearSearchPolicy<ValueType, CASE_SENSITIVE>;
//     using Comparison = LexicographicComparisonPolicy<ValueType, CASE_SENSITIVE>;
// };
//
// TRLC_ENUM_DEF(Permission, PowerOf2Policies,
//     Read = TRLC_FIELD(value = 1),    // 0x01
//     Write,                             // 0x02 (auto)
//     Execute,                           // 0x04 (auto)
//     Delete                             // 0x08 (auto)
// );
//
// int flags = Permission::Read.value() | Permission::Write.value();  // 0x03
// if (flags & Permission::Execute.value()) { /* has execute */ }

}  // namespace trlc::enums::policies