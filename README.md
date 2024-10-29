# TRLC Enum

A powerful C++ enum library that enhances traditional enums by allowing the use of attributes and providing support for compile-time operations.

## Features

### Enum Declaration

Declaration in the traditional way

```c++
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
```
