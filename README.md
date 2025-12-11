# TRLC Enum

![GCC](https://github.com/tranglecong/trlc_enum/actions/workflows/GCC.yml/badge.svg)
![Clang](https://github.com/tranglecong/trlc_enum/actions/workflows/Clang.yml/badge.svg)
![MSVC](https://github.com/tranglecong/trlc_enum/actions/workflows/MSVC.yml/badge.svg)

A modern C++17 header-only enum library with **compile-time operations**, rich metadata, STL integration, and customizable policies. All features work at compile-time with full `constexpr` support.

- **Full Compile-Time Support** - `constexpr` everything: lookup, iteration, validation
- **Rich Metadata** - Attach `value`, `desc`, `extra` fields to each enum element
- **Auto-Increment** - Automatic value filling with customizable increment policies
- **Powerful Lookup** - `fromValue()` and `fromName()` with `std::optional`
- **STL Integration** - Works seamlessly with containers, algorithms, and iterators
- **Custom Policies** - Define your own increment, search, and comparison behaviors
- **Type-Safe** - Strong typing with comparison operators and switch statement support
- **Header-Only** - No linking required, just include and use

## Basic Usage

### Declaration

We can declare enums in a traditional way, with fields or mix.

```c++
#include <trlc/enum.hpp>
```

```c++
// Basic enum
TRLC_ENUM(Color, Red, Yellow, Green);
```

```c++
// Enum with rich metadata: value, desc, extra
TRLC_ENUM(HttpStatus,
          OK = TRLC_FIELD(value = 200, desc = "Request successful", extra = "2xx"),
          Created = TRLC_FIELD(value = 201, desc = "Resource created successfully", extra = "2xx"),
          BadRequest = TRLC_FIELD(value = 400, desc = "Invalid request syntax", extra = "4xx"));
```

```c++
// We can declare enum constants with any combination of fields: value, desc, extra in any order
TRLC_ENUM(Fields,
          None,
          Value = TRLC_FIELD(value = 5),
          Desc = TRLC_FIELD(desc = "With Description"),
          Extra = TRLC_FIELD(extra = "Extra Info"),
          All = TRLC_FIELD(value = 10, desc = "All Fields", extra = "Full"));
```

```c++
// Enum with custom types: std::string_view for value, desc, extra
constexpr bool case_sensitive{false};
using FilePolicies = trlc::enums::policies::
    DefaultPolicies<std::string_view, std::string_view, std::string_view, case_sensitive>;
TRLC_ENUM_DEF(FileType,
              FilePolicies,
              Json = TRLC_FIELD(value = ".json", desc = "JSON file", extra = "application/json"),
              Xml = TRLC_FIELD(value = ".xml", desc = "XML file", extra = "application/xml"),
              Yaml = TRLC_FIELD(value = ".yaml", desc = "YAML file", extra = "application/x-yaml"))
```

> This Enum is essentially a struct, so we can declare it within the scope where the struct can be declared.

### Attributes

```c++
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
```

### Conversion

```c++
// We can use the functions fromValue and fromString to lookup the enum element.
// The return value will be constexpr std::optional<enum>
constexpr auto status_opt{HttpStatus::fromValue(201)};
static_assert(status_opt.has_value());
static_assert(status_opt.value() == HttpStatus::Created);

// Look up with case-insensitive
constexpr auto filetype_opt{FileType::fromName("XML")};
static_assert(filetype_opt.has_value());
static_assert(filetype_opt.value() == FileType::Xml);

std::cout << "Compile time fromValue(), fromString() check passed." << std::endl;
```

### Comparison

```c++
// We can use comparison operators
constexpr auto yellow{Color::Yellow};
static_assert(yellow != Color::Green);
static_assert(yellow > Color::Red);
static_assert(yellow < Color::Green);
static_assert(yellow == Color::Yellow);

std::cout << "Compile time comparison operators check passed." << std::endl;
```

### Iterators

```c++
// Range-based for loop
constexpr std::size_t num_colors = [] {
    std::size_t count = 0;
    for ([[maybe_unused]] const auto& light : Color::all()) {
        ++count;
    }
    return count;
}();
static_assert(num_colors == Color::SIZE);

std::cout << "Compile time range-based for loop check passed." << std::endl;
```

```c++
// Explicit iterators
std::cout << HttpStatus::NAME << " :\n";
for (auto it = HttpStatus::begin(); it != HttpStatus::end(); ++it) {
    std::cout << " - " << it->name() << " = " << it->value() << " (" << it->desc() << ", "
                << it->extra() << ")\n";
}
```

### STL Compatibility

```c++
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
```
**Terminal Output:**
```shell
Compile time attributes check passed.
Compile time fromValue(), fromString() check passed.
Compile time comparison operators check passed.
Compile time range-based for loop check passed.
HttpStatus :
 - OK = 200 (Request successful, 2xx)
 - Created = 201 (Resource created successfully, 2xx)
 - BadRequest = 400 (Invalid request syntax, 4xx)
Light Sequence: Green Red Yellow Green
```

> **You can see the full basic usage example at [[basic_usage](https://github.com/tranglecong/trlc_enum/blob/master/examples/basic_usage.cpp)]**

## Customization
### Custom Extra Metadata
```c++
// Use case: Store custom struct type as extra metadata (e.g., RGB color values)
// Features: Compile-time custom types, type-safe metadata, constexpr validation
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
```

```c++
// Custom extra field type with default policies
// NOTE: extra = (RGB{255, 0, 0}) syntax requires parentheses to avoid parsing issues
constexpr bool case_sensitive{false};
using ColorPolicies = trlc::enums::policies::DefaultPolicies<int, std::string_view, RGB, case_sensitive>;
TRLC_ENUM_DEF(Color,
              ColorPolicies,
              Red = TRLC_FIELD(extra = (RGB{255, 0, 0})),
              Green = TRLC_FIELD(extra = (RGB{0, 255, 0})),
              Blue = TRLC_FIELD(extra = (RGB{0, 0, 255})));
```

```c++
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
```

**Terminal Output**
```shell
Custom extra metadata RGB values verified at compile time.
Red: (255, 0, 0)
Green: (0, 255, 0)
Blue: (0, 0, 255)
```

> **You can see the full custom extra example at [[custom_extra](https://github.com/tranglecong/trlc_enum/blob/master/examples/custom_extra.cpp)]**

### Custom Policies

```c++
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
```

> **You can see the full basic usage example at [[basic_usage](https://github.com/tranglecong/trlc_enum/blob/master/examples/custom_policies.cpp)]**

## Installation

### Prerequisites

To use this library, you need:

- **CMake** 3.10 or higher
- **GCC**, **Clang** or **MSVC** compiler with C++17 support
- **GoogleTest** (automatically fetched by CMake for testing)

### Integration

#### Subdirectory

This library can be used as CMake subdirectory.

1. Fetch it, e.g. using [git submodules]:

```console
git submodule add https://github.com/tranglecong/trlc_enum
git submodule update --init --recursive
```

Or you can use git clone: `git clone https://github.com/tranglecong/trlc_enum.git`

1. Call `add_subdirectory(path_to/trlc_enum)` or whatever your local path is to make it available in [CMake] file.

2. Simply call `target_link_libraries(your_target PUBLIC trlc::enum)` to link this library and setups the include search path and compilation options.

#### Install Library

You can also install trlc_enum library

1. Run CMake configure inside the library sources. If you want to build the UT and example set `-DTRLC_BUILD_TESTS=ON` , `-DTRLC_BUILD_EXAMPLES=ON`

    ```bash
    cmake -DCMAKE_BUILD_TYPE=Debug -DTRLC_BUILD_TESTS=OFF -DTRLC_BUILD_EXAMPLES=OFF -DTRLC_GENERATE_RECURSIVE_MACRO=ON -S . -B ./build
    ```

> The Enum library uses a recursive macro. The header macros will be generated when running the CMake configure through the execute Python script [[macro_expansion_generator.py](https://github.com/tranglecong/trlc_enum/blob/master/macro_expansion_generator.py)]. The default value of `TRLC_MACRO_RECURSIVE_MAX_NUM` is 64. If you want to change it, you can modify the CMake file or set `-DTRLC_MACRO_RECURSIVE_MAX_NUM=xxx`. With xxx being the number you desire.

1. Build and install the library under `${CMAKE_INSTALL_PREFIX}`. You may be required to have sudo privileges to install in the `/usr/*`.

    ```bash
    cmake --build ./build -j8 -- install
    ```

    [Optional] if you want to run UT.

    ```bash
    ctest --test-dir ./build
    ```

2. To use an installed library.

    ```cmake
    find_package(trlc REQUIRED)
    target_link_libraries(your_target PUBLIC trlc::enum)
    ```

## Contributing

Welcome contributions from everyone! If you’d like to help improve this project.
Thank you for considering contributing to this project!
