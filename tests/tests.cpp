#include "trlc/enum.hpp" // Include your enum library header

#include <gtest/gtest.h>

// Define new enums for testing
TRLC_ENUM(Colors,
          RED,
          GREEN,
          BLUE,
          YELLOW)

TRLC_ENUM(Vehicles,
          CAR = TRLC_FIELD(default = 1, desc = "A four-wheeled motor vehicle."),
          TRUCK = TRLC_FIELD(default = 2, desc = "A motor vehicle designed to transport cargo."),
          MOTORCYCLE = TRLC_FIELD(default = 3, desc = "A two-wheeled motor vehicle."),
          BICYCLE = TRLC_FIELD(default = 4, desc = "A human-powered vehicle with two wheels."))

TRLC_ENUM(StatusCodes,
          SUCCESS = TRLC_FIELD(default = 0, desc = "Operation completed successfully."),
          ERROR = TRLC_FIELD(default = 1, desc = "An error occurred."),
          PENDING = TRLC_FIELD(default = 2, desc = "Operation pending."))

TRLC_ENUM(Validate,
          NON_FIELD,
          WITH_DEFAULT = TRLC_FIELD(default = 5),
          WITH_DESC = TRLC_FIELD(desc = "With description."),
          FULL_FIELD = TRLC_FIELD(default = 100, desc = "Full feild."),
          NEGATIVE_VALUE = TRLC_FIELD(default = -100, desc = "Default trlc enum can support negative value."),
          END)

// Tests for Validate Enum
TEST(ValidateTest, ValidateAttributes)
{
    EXPECT_EQ(Validate::NON_FIELD.value(), 0);
    EXPECT_EQ(Validate::WITH_DEFAULT.value(), 5);
    EXPECT_EQ(Validate::WITH_DESC.name(), "WITH_DESC");
    EXPECT_EQ(Validate::WITH_DESC.desc(), "With description.");
    EXPECT_EQ(Validate::FULL_FIELD.value(), 100);
    EXPECT_EQ(Validate::FULL_FIELD.desc(), "Full feild.");
    EXPECT_EQ(Validate::NEGATIVE_VALUE.value(), -100);
    EXPECT_EQ(Validate::NEGATIVE_VALUE.desc(), "Default trlc enum can support negative value.");
}

TEST(ValidateFunctionalTest, FromValueFunctionality)
{
    EXPECT_EQ(Validate::fromValue(0).value(), Validate::NON_FIELD); // Non-field should be 0 by default
    EXPECT_EQ(Validate::fromValue(5).value(), Validate::WITH_DEFAULT);
    EXPECT_EQ(Validate::fromValue(100).value(), Validate::FULL_FIELD);
    EXPECT_EQ(Validate::fromValue(-100).value(), Validate::NEGATIVE_VALUE);

    // Test for values not defined in the enum
    auto invalidOptional = Validate::fromValue(99);
    ASSERT_FALSE(invalidOptional.has_value());
}

TEST(ValidateIteratorTest, ValidateIteratorCount)
{
    size_t count = 0;
    for (auto elem : Validate::iterator)
    {
        count++;
    }
    EXPECT_EQ(count, Validate::size()); // Total enum members should match
}

TEST(ValidateDumpFunctionTest, ValidateDumpProperties)
{
    EXPECT_NO_THROW(std::cout << Validate::dump());

    // Validate dump for Validate
    auto validateDump = Validate::dump();
    EXPECT_NE(validateDump.find("NON_FIELD"), std::string::npos);
    EXPECT_NE(validateDump.find("WITH_DEFAULT"), std::string::npos);
    EXPECT_NE(validateDump.find("WITH_DESC"), std::string::npos);
    EXPECT_NE(validateDump.find("FULL_FIELD"), std::string::npos);
    EXPECT_NE(validateDump.find("NEGATIVE_VALUE"), std::string::npos);

    // Validate descriptions
    EXPECT_NE(validateDump.find(Validate::WITH_DESC.desc()), std::string::npos);
    EXPECT_NE(validateDump.find(Validate::NEGATIVE_VALUE.desc()), std::string::npos);
}

TEST(ValidateEnumComparisonTest, EnumComparisons)
{
    EXPECT_EQ(Validate::NON_FIELD, Validate::NON_FIELD);
    EXPECT_NE(Validate::WITH_DEFAULT, Validate::WITH_DESC);
    EXPECT_GT(Validate::FULL_FIELD.value(), Validate::WITH_DEFAULT.value());
    EXPECT_LT(Validate::NEGATIVE_VALUE.value(), Validate::WITH_DEFAULT.value());
}

TEST(ValidateFromStringTest, ValidStrings)
{
    EXPECT_TRUE(Validate::fromString("NON_FIELD") == Validate::NON_FIELD);
    EXPECT_TRUE(Validate::fromString("WITH_DEFAULT") == Validate::WITH_DEFAULT);
    EXPECT_TRUE(Validate::fromString("WITH_DESC") == Validate::WITH_DESC);
    EXPECT_TRUE(Validate::fromString("FULL_FIELD") == Validate::FULL_FIELD);
    EXPECT_TRUE(Validate::fromString("NEGATIVE_VALUE") == Validate::NEGATIVE_VALUE);
}

TEST(ValidateFromStringTest, InvalidStrings)
{
    EXPECT_FALSE(Validate::fromString("INVALID_NAME").has_value());
    EXPECT_FALSE(Validate::fromString("").has_value());
    EXPECT_FALSE(Validate::fromString("123").has_value());
    EXPECT_FALSE(Validate::fromString("WITH DESC").has_value()); // String contains a space
}

TEST(ColorsTest, ColorAttributes)
{
    EXPECT_EQ(Colors::RED.value(), 0);
    EXPECT_EQ(Colors::GREEN.name(), "GREEN");
    EXPECT_EQ(Colors::YELLOW.value(), 3);
    EXPECT_EQ(Colors::BLUE.name(), "BLUE");
}

TEST(VehiclesTest, VehicleAttributes)
{
    EXPECT_EQ(Vehicles::CAR.value(), 1);
    EXPECT_EQ(Vehicles::TRUCK.desc(), "A motor vehicle designed to transport cargo.");
    EXPECT_EQ(Vehicles::MOTORCYCLE.value(), 3);
    EXPECT_EQ(Vehicles::BICYCLE.name(), "BICYCLE");
}

TEST(StatusCodesTest, StatusAttributes)
{
    EXPECT_EQ(StatusCodes::SUCCESS.value(), 0);
    EXPECT_EQ(StatusCodes::ERROR.desc(), "An error occurred.");
    EXPECT_EQ(StatusCodes::PENDING.value(), 2);
}

TEST(EnumFunctionalTest, FromValueFunctionality)
{
    constexpr auto greenOptional = Colors::fromValue(1); // Should yield GREEN
    ASSERT_TRUE(greenOptional.has_value());
    EXPECT_EQ(greenOptional.value(), Colors::GREEN);

    constexpr auto carOptional = Vehicles::fromValue(1);
    ASSERT_TRUE(carOptional.has_value());
    EXPECT_EQ(carOptional.value(), Vehicles::CAR);

    constexpr auto invalidOptional = Vehicles::fromValue(99); // Assuming 99 does not exist
    ASSERT_FALSE(invalidOptional.has_value());
}

TEST(EnumIteratorTest, VehicleIterator)
{
    size_t count = 0;
    for (auto elem : Vehicles::iterator)
    {
        count++;
    }
    EXPECT_EQ(count, 4); // Total vehicle types
}

TEST(EnumHolderTest, HolderProperties)
{
    constexpr auto carHolder = Vehicles::fromValue(1).value();
    EXPECT_EQ(carHolder.tag(), "Vehicles");
    EXPECT_EQ(carHolder.holder().tag(), "Vehicles");
    ASSERT_EQ(carHolder.holder().MOTORCYCLE, Vehicles::MOTORCYCLE);
}

TEST(EnumDumpFunctionTest, DumpProperties)
{
    EXPECT_NO_THROW(std::cout << Colors::dump());
    EXPECT_NO_THROW(std::cout << Vehicles::dump());
    EXPECT_NO_THROW(std::cout << StatusCodes::dump());

    // Validate dump for Colors
    auto colorsDump = Colors::dump();
    EXPECT_NE(colorsDump.find("RED"), std::string::npos);
    EXPECT_NE(colorsDump.find("BLUE"), std::string::npos);

    // Validate dump for Vehicles
    auto vehiclesDump = Vehicles::dump();
    EXPECT_NE(vehiclesDump.find("CAR"), std::string::npos);
    EXPECT_NE(vehiclesDump.find("TRUCK"), std::string::npos);

    // Validate descriptions
    EXPECT_NE(vehiclesDump.find(Vehicles::CAR.desc()), std::string::npos);
    EXPECT_NE(vehiclesDump.find(Vehicles::TRUCK.desc()), std::string::npos);
}

TEST(EnumIterateAndDumpTest, StatusCodesIterateAndDump)
{
    for (auto elem : StatusCodes::iterator)
    {
        EXPECT_FALSE(elem.tag().empty());
        EXPECT_GE(elem.value(), 0); // assuming non-negative values
        EXPECT_FALSE(elem.name().empty());
        EXPECT_FALSE(elem.desc().empty());
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}