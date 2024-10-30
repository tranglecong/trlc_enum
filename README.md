# TRLC Enum

A powerful C++ enum library that enhances traditional enums by allowing the use of attributes and providing support for compile-time operations.

## Features

> **Declaration:** We can declare enums in a traditional way, with fields or mix.

```c++
#include <trlc/enum.hpp>

TRLC_ENUM(Rainbow,
          RED,
          ORANCE,
          YELLOW,
          GREEN,
          BLUE,
          INDIGO,
          VIOLET)
```

```c++
TRLC_ENUM(Cars,
          SEDAN = TRLC_FIELD(value = 1, desc = "A comfortable car for daily commuting and family trips."),
          SUV = TRLC_FIELD(value = 2, desc = "A versatile vehicle built for various terrains and passenger capacity."),
          TRUCK = TRLC_FIELD(value = 3, desc = "A powerful vehicle designed for transporting heavy loads and equipment."),
          JEEP = TRLC_FIELD(value = 4, desc = "A rugged vehicle ideal for off-road adventures and exploration."))
```

```c++
TRLC_ENUM(Validate,
          NON_FIELD,
          WITH_DEFAULT = TRLC_FIELD(value = 5),
          WITH_DESC = TRLC_FIELD(desc = "With description."),
          FULL_FIELD = TRLC_FIELD(value = 100, desc = "Full feild."),
          NEGATIVE_VALUE = TRLC_FIELD(value = -100, desc = "Default trlc enum can support negative value."),
          END)
```

> **Attributes:** We can use the attributes `name`, `value`, `desc` and `tag` of trlc enum

```c++
static_assert(Rainbow::ORANCE.tag() == "Rainbow");
static_assert(Rainbow::RED.value() == 0);
static_assert(Rainbow::GREEN.name() == "GREEN");
static_assert(Cars::JEEP.value() == 4);
static_assert(Cars::SUV.name() == "SUV");
static_assert(Cars::SEDAN.desc() == "A comfortable car for daily commuting and family trips.");
static_assert(Validate::NEGATIVE_VALUE.name() == "NEGATIVE_VALUE");
static_assert(Validate::NEGATIVE_VALUE.value() == -100);
static_assert(Validate::NEGATIVE_VALUE.desc() == "Default trlc enum can support negative value.");

std::cout << "Compile time attributes check passed." << std::endl;
```

> **Conversion:** We can use the fromValue and fromString functions to look up enum elements.

```c++
constexpr auto rainbow_green_optional{Rainbow::fromValue(3)};
static_assert(rainbow_green_optional.has_value() == true);
static_assert(rainbow_green_optional.value() == Rainbow::GREEN);

constexpr auto cars_suv_optional{Cars::fromString("SUV")};
static_assert(cars_suv_optional.has_value() == true);
static_assert(cars_suv_optional.value() == Cars::SUV);

std::cout << "Compile time fromValue(), fromString() check passed." << std::endl;
```

_The return value will be constexpr `std::optional<enumtype>`._

> **Iterators:** We can also use iterators for enum classes. And it also supports compile-time!

```c++
constexpr auto check_size_of_rainbow = [&]() -> size_t
{
    auto size{0};
    for (auto elem : Rainbow::iterator)
    {
        size++;
    }
    return size;
};
static_assert(check_size_of_rainbow() == Rainbow::size());

std::cout << "Compile time iterators check passed." << std::endl;
```

> **Traceability:** From an enum element, we can also retrieve its holder.

```c++
constexpr auto suv{cars_suv_optional.value()};
static_assert(suv.tag() == "Cars");
static_assert(suv.holder().tag() == "Cars");
static_assert(suv.holder().TRUCK == Cars::TRUCK);

std::cout << "Compile time holder check passed." << std::endl;
```

_The tag property of the enum or enum element is the name of that enum._

> **Information:** Each enum or enum element has a dump() function that operates at runtime.

```c++
std::cout << "[1] Enum Rainbow :";
std::cout << Rainbow::dump() << std::endl;

std::cout << "[2] Enum Cars :";
std::cout << Cars::dump() << std::endl;

std::cout << "[3] Enum Validate :\n";
// Of course, we can also use iterators to print the properties.
for (auto elem : Validate::iterator)
{
    std::cout << elem.dump() << std::endl;
}
```

_It returns a JSON string of the properties associated with the enum element and a list of elements for the enum._

> **Customization:** Currently, `TRLC_ENUM` uses `trlc:DefaultEnumDef<>`, but you can also define an enum definition and use it with `TRLC_ENUM_DETAIL`.

```c++
template<class Holder>
struct CustomEnumDefine
{
    using holder = Holder;
    using value_type = uint32_t;
    using value_search_policy = trlc::policy::BinarySearchPolicy;
    using name_search_policy = trlc::policy::CaseInsensitiveStringSearchPolicy;
    using unknown_policy = trlc::policy::UnknownPolicy;
    using enum_type = trlc::Enum<value_type, holder>;
    using iterator = trlc::EnumIterator<holder>;
};

TRLC_ENUM_DETAIL(Colors, CustomEnumDefine,
    RED,
    BLUE,
    GREEN)
```

The output of running all the blocks above will be as follows:

```console
trlc@UFO:~/projects/trlc_enum$ ./build/example/example
Compile time attributes check passed.
Compile time fromValue(), fromString() check passed.
Compile time iterators check passed.
Compile time holder check passed.
[1] Enum Rainbow :[
 {"tag": "Rainbow, "value": 0, "name": "RED", "desc": ""},
 {"tag": "Rainbow, "value": 1, "name": "ORANCE", "desc": ""},
 {"tag": "Rainbow, "value": 2, "name": "YELLOW", "desc": ""},
 {"tag": "Rainbow, "value": 3, "name": "GREEN", "desc": ""},
 {"tag": "Rainbow, "value": 4, "name": "BLUE", "desc": ""},
 {"tag": "Rainbow, "value": 5, "name": "INDIGO", "desc": ""},
 {"tag": "Rainbow, "value": 6, "name": "VIOLET", "desc": ""},
]
[2] Enum Cars :[
 {"tag": "Cars, "value": 1, "name": "SEDAN", "desc": "A comfortable car for daily commuting and family trips."},
 {"tag": "Cars, "value": 2, "name": "SUV", "desc": "A versatile vehicle built for various terrains and passenger capacity."},
 {"tag": "Cars, "value": 3, "name": "TRUCK", "desc": "A powerful vehicle designed for transporting heavy loads and equipment."},
 {"tag": "Cars, "value": 4, "name": "JEEP", "desc": "A rugged vehicle ideal for off-road adventures and exploration."},
]
[3] Enum Validate :
{"tag": "Validate, "value": -100, "name": "NEGATIVE_VALUE", "desc": "Default trlc enum can support negative value."}
{"tag": "Validate, "value": 0, "name": "NON_FIELD", "desc": ""}
{"tag": "Validate, "value": 1, "name": "WITH_DESC", "desc": "With description."}
{"tag": "Validate, "value": 2, "name": "END", "desc": ""}
{"tag": "Validate, "value": 5, "name": "WITH_DEFAULT", "desc": ""}
{"tag": "Validate, "value": 100, "name": "FULL_FIELD", "desc": "Full feild."}
```

You can see the full example at [[example](https://github.com/tranglecong/trlc_enum/blob/master/example/example.cpp)]

## Installation

### Prerequisites

To use this library, you need:

- **CMake** 3.10 or higher
- **GCC**, **Clang** or **MSVC** compiler with C++17 support
- **GoogleTest** (automatically fetched by CMake for testing)

### Intergration

> **Subdirectory:** _This library can be used as [CMake] subdirectory_

1. Fetch it, e.g. using [git submodules] `git submodule add https://github.com/tranglecong/trlc_enum` and `git submodule update --init --recursive`.

2. Call `add_subdirectory(ext/trlc_enum)` or whatever your local path is to make it available in [CMake].

3. Simply call `target_link_libraries(your_target PUBLIC [trlc::enum, trlc_enum])` to link this library and setups the include search path and compilation options.

> **Install:** _You can also install trlc_enum library_

1. Run CMake configure inside the library sources. If you do not want to build the UT and example set `-DTRLC_BUILD_TESTS=OFF` , `-DTRLC_BUILD_EXAMPLES=OFF`

    ```bash
    cmake -DCMAKE_BUILD_TYPE=Debug -DTRLC_BUILD_TESTS=ON -DTRLC_BUILD_EXAMPLES=ON -DTRLC_GENERATE_RECURSIVE_MACRO=ON -S . -B ./build
    ```

The Enum library uses a recursive macro. The header macros will be generated when running the CMake configure through the execute Python script [[macro_expansion_generator.py](https://github.com/tranglecong/trlc_enum/blob/master/macro_expansion_generator.py)]. The default value of `TRLC_MACRO_RECURSIVE_MAX_NUM` is 64. If you want to change it, you can modify the CMake file or set `-DTRLC_MACRO_RECURSIVE_MAX_NUM=xxx`. With xxx being the number you desire.

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
