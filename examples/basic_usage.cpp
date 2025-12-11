#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include <trlc/enum.hpp>
#include <trlc/enums/policies.hpp>

// Basic enum
TRLC_ENUM(Color, Red, Yellow, Green);

// Enum with rich metadata: value, desc, extra
TRLC_ENUM(HttpStatus,
          OK = TRLC_FIELD(value = 200, desc = "Request successful", extra = "2xx"),
          Created = TRLC_FIELD(value = 201, desc = "Resource created successfully", extra = "2xx"),
          BadRequest = TRLC_FIELD(value = 400, desc = "Invalid request syntax", extra = "4xx"));

// We can declare enum constants with any combination of fields: value, desc, extra in any order
TRLC_ENUM(Fields,
          None,
          Value = TRLC_FIELD(value = 5),
          Desc = TRLC_FIELD(desc = "With Description"),
          Extra = TRLC_FIELD(extra = "Extra Info"),
          All = TRLC_FIELD(value = 10, desc = "All Fields", extra = "Full"));

// Enum with custom types: std::string_view for value, desc, extra
constexpr bool case_sensitive{false};
using FilePolicies = trlc::enums::policies::
    DefaultPolicies<std::string_view, std::string_view, std::string_view, case_sensitive>;
TRLC_ENUM_DEF(FileType,
              FilePolicies,
              Json = TRLC_FIELD(value = ".json", desc = "JSON file", extra = "application/json"),
              Xml = TRLC_FIELD(value = ".xml", desc = "XML file", extra = "application/xml"),
              Yaml = TRLC_FIELD(value = ".yaml", desc = "YAML file", extra = "application/x-yaml"));

int main() {
    // We can use attributes like name(), value(), desc(), extra(), enumName() on enum instances
    static_assert(HttpStatus::OK.value() == 200);
    static_assert(HttpStatus::Created.desc() == "Resource created successfully");
    static_assert(HttpStatus::BadRequest.extra() == "4xx");
    static_assert(HttpStatus::OK.enumName() == "HttpStatus");

    constexpr auto green{Color::Green};
    static_assert(green.name() == "Green");
    static_assert(green.value() == 2);
    static_assert(green.enumName() == "Color");

    std::cout << "Compile time attributes check passed." << std::endl;

    // We can use the functions fromValue and fromString to lookup the enum element.
    // The return value will be constexpr std::optional<enum>
    constexpr auto status_opt{HttpStatus::fromValue(201)};
    static_assert(status_opt.has_value());
    static_assert(status_opt.value() == HttpStatus::Created);

    // Look up with case-insensitive (runtime check since fromName isn't fully constexpr)
    auto filetype_opt{FileType::fromName("XML")};
    assert(filetype_opt.has_value());
    assert(filetype_opt.value() == FileType::Xml);

    std::cout << "Compile time fromValue(), fromString() check passed." << std::endl;

    // We can use comparison operators
    constexpr auto yellow{Color::Yellow};
    static_assert(yellow != Color::Green);
    static_assert(yellow > Color::Red);
    static_assert(yellow < Color::Green);
    static_assert(yellow == Color::Yellow);

    std::cout << "Compile time comparison operators check passed." << std::endl;

    // Range-based for loop
    constexpr std::size_t num_colors = [] {
        std::size_t count = 0;
        for ([[maybe_unused]] const auto& color : Color::all()) {
            ++count;
        }
        return count;
    }();
    static_assert(num_colors == Color::SIZE);

    std::cout << "Compile time range-based for loop check passed." << std::endl;

    // Explicit iterators
    std::cout << HttpStatus::NAME << " :\n";
    for (auto it = HttpStatus::begin(); it != HttpStatus::end(); ++it) {
        std::cout << " - " << it->name() << " = " << it->value() << " (" << it->desc() << ", "
                  << it->extra() << ")\n";
    }

    // STL containers compatibility
    auto found =
        std::find_if(Fields::begin(), Fields::end(), [](auto f) { return f == Fields::All; });

    assert(found != Fields::end());
    assert(*found == Fields::All);

    std::vector<Color::Enum> lights{};
    lights.push_back(Color::Green);
    lights.push_back(Color::Red);
    lights.push_back(Color::Yellow);
    lights.push_back(Color::Green);

    std::cout << "Light Sequence: ";
    for (const auto& light : lights) {
        std::cout << light.name() << " ";
    }

    return 0;
}