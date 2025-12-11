#include <iostream>

#include "trlc/enum.hpp"

using namespace trlc::enums::policies;

// Use case: Bit flags for permissions, options, etc.
// Features: Auto-increment as powers of 2 (1, 2, 4, 8, ...)

template <typename T>
struct PowerOf2Increment {
    static constexpr T startValue() { return 0x01; }

    static constexpr T increment(T current, std::size_t /* index */) { return current * 2; }
};

template <typename T>
struct PowerOf2Policies {
    using ValueType = T;
    using DescType = std::string_view;
    using ExtraType = std::string_view;
    inline static constexpr bool CASE_SENSITIVE{true};

    using Increment = PowerOf2Increment<ValueType>;
    using Search = LinearSearchPolicy<ValueType, CASE_SENSITIVE>;
    using Comparison = LexicographicComparisonPolicy<ValueType, CASE_SENSITIVE>;
};

TRLC_ENUM_DEF(Permission,
              PowerOf2Policies<uint32_t>,
              Read,     // 0x01 (auto)
              Write,    // 0x02 (auto)
              Execute,  // 0x04 (auto)
              Delete    // 0x08 (auto)
);

TRLC_ENUM(Words,
          Read,    
          Write,   
          Execute = TRLC_FIELD(value = 42), 
          Delete = TRLC_FIELD(value = 1),
          A,
          B  
);

int main() {
    constexpr auto flags = Permission::Read.value() | Permission::Write.value();

    static_assert((flags & (Permission::Read.value() | Permission::Write.value())) != 0U,
                  "Flags must include Read and Write");

    if ((flags & Permission::Read.value()) != 0U) { /* has read */
        std::cout << "Read permission granted.\n";
    }

    if ((flags & Permission::Write.value()) != 0U) { /* has write */
        std::cout << "Write permission granted.\n";
    }
    if ((flags & Permission::Execute.value()) != 0U) { /* has execute */
        std::cout << "Execute permission granted.\n";
    }
    if ((flags & Permission::Delete.value()) != 0U) { /* has delete */
        std::cout << "Delete permission granted.\n";
    }

    for(auto word : Words::all()) {
        std::cout << word.name() << " = " << word.value() << std::endl;
    }

    return 0;
}