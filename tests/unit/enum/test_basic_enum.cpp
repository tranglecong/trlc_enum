#include <gtest/gtest.h>
#include "trlc/enum.hpp"

namespace {

// Simple enum without TRLC_FIELD
TRLC_ENUM(Color, Red, Green, Blue, Yellow);

// Enum with more elements
TRLC_ENUM(Direction, North, South, East, West);

TEST(BasicEnumTest, EnumSize) {
    EXPECT_EQ(Color::SIZE, 4);
    EXPECT_EQ(Direction::SIZE, 4);
}

TEST(BasicEnumTest, EnumConstants) {
    // Test that enum constants are accessible
    auto red = Color::Red;
    auto green = Color::Green;
    auto blue = Color::Blue;
    auto yellow = Color::Yellow;
    
    EXPECT_EQ(red.index(), 0);
    EXPECT_EQ(green.index(), 1);
    EXPECT_EQ(blue.index(), 2);
    EXPECT_EQ(yellow.index(), 3);
}

TEST(BasicEnumTest, EnumNames) {
    EXPECT_EQ(Color::datas[0].name, "Red");
    EXPECT_EQ(Color::datas[1].name, "Green");
    EXPECT_EQ(Color::datas[2].name, "Blue");
    EXPECT_EQ(Color::datas[3].name, "Yellow");
}

TEST(BasicEnumTest, EnumValues) {
    // Default values are integers 0, 1, 2, 3
    EXPECT_EQ(Color::datas[0].value, 0);
    EXPECT_EQ(Color::datas[1].value, 1);
    EXPECT_EQ(Color::datas[2].value, 2);
    EXPECT_EQ(Color::datas[3].value, 3);
}

TEST(BasicEnumTest, EnumInstanceName) {
    auto red = Color::Red;
    EXPECT_EQ(red.name(), "Red");
    
    auto north = Direction::North;
    EXPECT_EQ(north.name(), "North");
}

TEST(BasicEnumTest, EnumInstanceValue) {
    auto red = Color::Red;
    EXPECT_EQ(red.value(), 0);  // Default int values
    
    auto south = Direction::South;
    EXPECT_EQ(south.value(), 1);  // Default increment: 0,1,2,3
}

TEST(BasicEnumTest, EnumInstanceIndex) {
    EXPECT_EQ(Color::Red.index(), 0);
    EXPECT_EQ(Color::Green.index(), 1);
    EXPECT_EQ(Color::Blue.index(), 2);
    EXPECT_EQ(Color::Yellow.index(), 3);
}

TEST(BasicEnumTest, EnumComparison) {
    auto red1 = Color::Red;
    auto red2 = Color::Red;
    auto green = Color::Green;
    
    EXPECT_TRUE(red1 == red2);
    EXPECT_FALSE(red1 != red2);
    EXPECT_TRUE(red1 != green);
    EXPECT_FALSE(red1 == green);
}

TEST(BasicEnumTest, EnumAssignment) {
    // Test copy assignment
    Color::Enum color = Color::Red;
    EXPECT_EQ(color, Color::Red);
    EXPECT_EQ(color.name(), "Red");
    
    // Reassign to different value
    color = Color::Green;
    EXPECT_EQ(color, Color::Green);
    EXPECT_EQ(color.name(), "Green");
    
    // Multiple reassignments
    color = Color::Blue;
    EXPECT_EQ(color.name(), "Blue");
    color = Color::Yellow;
    EXPECT_EQ(color.name(), "Yellow");
}

TEST(BasicEnumTest, EnumDefaultConstruct) {
    // Default construction gives index() = 0
    Color::Enum color;
    EXPECT_EQ(color.index(), 0);
    EXPECT_EQ(color.name(), "Red");
    
    // Can assign after default construction
    color = Color::Blue;
    EXPECT_EQ(color.index(), 2);
    EXPECT_EQ(color.name(), "Blue");
}

TEST(BasicEnumTest, ConstexprUsage) {
    constexpr auto red = Color::Red;
    constexpr auto idx = red.index();
    static_assert(idx == 0, "Red should be at index 0");
    
    constexpr auto size = Color::SIZE;
    static_assert(size == 4, "Color should have 4 elements");
}

TEST(BasicEnumTest, ArrayAccess) {
    // Access names array
    for (std::size_t i = 0; i < Color::SIZE; ++i) {
        EXPECT_FALSE(Color::datas[i].name.empty());
    }
    
    // Access values array (int values)
    EXPECT_EQ(Color::datas[0].value, 0);
    EXPECT_EQ(Color::datas[1].value, 1);
    EXPECT_EQ(Color::datas[2].value, 2);
    EXPECT_EQ(Color::datas[3].value, 3);
}

} // namespace
