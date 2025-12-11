#include <iostream>
#include <cstdint>

#include "trlc/enum.hpp"

// Use case: Store custom struct type as extra metadata (e.g., RGB color values)
// Features: Compile-time custom types, type-safe metadata, constexpr validation
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Custom extra field type with default policies
// NOTE: extra = (RGB{255, 0, 0}) syntax requires parentheses to avoid parsing issues
constexpr bool case_sensitive{false};
using ColorPolicies = trlc::enums::policies::DefaultPolicies<int, std::string_view, RGB, case_sensitive>;
TRLC_ENUM_DEF(Color,
              ColorPolicies,
              Red = TRLC_FIELD(extra = (RGB{255, 0, 0})),
              Green = TRLC_FIELD(extra = (RGB{0, 255, 0})),
              Blue = TRLC_FIELD(extra = (RGB{0, 0, 255})));

int main() {
    // Extra Metadata can work as normal struct
    static_assert(Color::Red.extra().r == 255);
    static_assert(Color::Red.extra().g == 0);
    static_assert(Color::Red.extra().b == 0);

    static_assert(Color::Green.extra().r == 0);
    static_assert(Color::Green.extra().g == 255);
    static_assert(Color::Green.extra().b == 0);

    static_assert(Color::Blue.extra().r == 0);
    static_assert(Color::Blue.extra().g == 0);
    static_assert(Color::Blue.extra().b == 255);

    std::cout << "Custom extra metadata RGB values verified at compile time." << std::endl;

    // Iterate over all Color enum values and print their names and RGB extras
    for (const auto& p : Color::all()) {
        std::cout << p.name() << ": (" << static_cast<int>(p.extra().r) << ", "
                  << static_cast<int>(p.extra().g) << ", " << static_cast<int>(p.extra().b)
                  << ")\n";
    }
}
