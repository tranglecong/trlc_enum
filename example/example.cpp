#include "trlc/enum.hpp"

#include <cstddef>
#include <iostream>

TRLC_ENUM(Rainbow,
          RED,
          ORANCE,
          YELLOW,
          GREEN,
          BLUE,
          INDIGO,
          VIOLET)

TRLC_ENUM(Cars,
          SEDAN = TRLC_FIELD(default = 1, desc = "A comfortable car for daily commuting and family trips."),
          SUV = TRLC_FIELD(default = 2, desc = "A versatile vehicle built for various terrains and passenger capacity."),
          TRUCK = TRLC_FIELD(default = 3, desc = "A powerful vehicle designed for transporting heavy loads and equipment."),
          JEEP = TRLC_FIELD(default = 4, desc = "A rugged vehicle ideal for off-road adventures and exploration."))

TRLC_ENUM(Validate,
          NON_FIELD,
          WITH_DEFAULT = TRLC_FIELD(default = 5),
          WITH_DESC = TRLC_FIELD(desc = "With description."),
          FULL_FIELD = TRLC_FIELD(default = 100, desc = "Full feild."),
          NEGATIVE_VALUE = TRLC_FIELD(default = -100, desc = "Default trlc enum can support negative value."),
          END)

int main()
{
    // We can use the attributes name, value, and desc of trlc enum.
    static_assert(Rainbow::RED.value() == 0);
    static_assert(Rainbow::GREEN.name() == "GREEN");
    static_assert(Cars::JEEP.value() == 4);
    static_assert(Cars::SUV.name() == "SUV");
    static_assert(Cars::SEDAN.desc() == "A comfortable car for daily commuting and family trips.");
    static_assert(Validate::NEGATIVE_VALUE.name() == "NEGATIVE_VALUE");
    static_assert(Validate::NEGATIVE_VALUE.value() == -100);
    static_assert(Validate::NEGATIVE_VALUE.desc() == "Default trlc enum can support negative value.");

    std::cout << "Compile time attributes check passed." << std::endl;

    // From the enum, we can use the functions fromValue and fromString to search for the enum element.
    // The return value will be constexpr std::optional<enum>
    constexpr auto rainbow_green_optional{Rainbow::fromValue(3)};
    static_assert(rainbow_green_optional.has_value() == true);
    static_assert(rainbow_green_optional.value() == Rainbow::GREEN);

    constexpr auto cars_suv_optional{Cars::fromString("SUV")};
    static_assert(cars_suv_optional.has_value() == true);
    static_assert(cars_suv_optional.value() == Cars::SUV);

    std::cout << "Compile time fromValue(), fromString() check passed." << std::endl;

    // We can also use iterators for enum classes. And it also supports compile-time!
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

    // From an enum element, we can also retrieve its holder.
    // The tag property of the enum or enum element is the name of that enum.
    constexpr auto suv{cars_suv_optional.value()};
    static_assert(suv.tag() == "Cars");
    static_assert(suv.holder().tag() == "Cars");
    static_assert(suv.holder().TRUCK == Cars::TRUCK);

    std::cout << "Compile time holder check passed." << std::endl;

    // Each enum or enum element has a dump() function that operates at runtime. It returns a JSON string of the properties associated with the enum element and a list of elements for the enum.

    std::cout << "[1] Enum Rainbow :";
    std::cout << Rainbow::dump() << std::endl;

    std::cout << "[2] Enum Cars :";
    std::cout << Cars::dump() << std::endl;

    std::cout << "[3] Enum Validate :\n";
    // Of course, we can also use iterators to print the properties.
    for (auto elem : Validate::iterator)
    {
        std::cout << "tag: " << elem.tag() << ", value: " << elem.value() << ", name: " << elem.name() << ", desc: " << elem.desc() << std::endl;
    }
}
