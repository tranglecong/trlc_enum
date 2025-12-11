#include <gtest/gtest.h>
#include "trlc/enum.hpp"

namespace {

// Custom increment policy for powers of 2
struct PowerOf2Increment {
    static constexpr int startValue() { return 1; }
    static constexpr int increment(int current, std::size_t /* index */) { return current * 2; }
};

// Custom policies with PowerOf2 increment
struct PowerOf2Policies {
    using ValueType = int;
    using DescType = std::string_view;
    using ExtraType = std::string_view;
    
    inline static constexpr bool CASE_SENSITIVE{true};
    
    using Increment = PowerOf2Increment;
    using Search = trlc::enums::policies::LinearSearchPolicy<ValueType, CASE_SENSITIVE>;
    using Comparison = trlc::enums::policies::LexicographicComparisonPolicy<ValueType, CASE_SENSITIVE>;
};

TRLC_ENUM_DEF(PowerEnum, PowerOf2Policies,
    First = TRLC_FIELD(value = 1),
    Second,  // Should be 2
    Third,   // Should be 4
    Fourth   // Should be 8
);

// Case-insensitive policies for string values
struct CaseInsensitivePolicies {
    using ValueType = std::string_view;
    using DescType = std::string_view;
    using ExtraType = std::string_view;
    
    inline static constexpr bool CASE_SENSITIVE{false};
    
    using Increment = trlc::enums::policies::DefaultIncrementPolicy<ValueType>;
    using Search = trlc::enums::policies::LinearSearchPolicy<ValueType, CASE_SENSITIVE>;
    using Comparison = trlc::enums::policies::LexicographicComparisonPolicy<ValueType, CASE_SENSITIVE>;
};

TRLC_ENUM_DEF(Command, CaseInsensitivePolicies,
    Start = TRLC_FIELD(value = "start"),
    Stop = TRLC_FIELD(value = "stop"),
    Restart = TRLC_FIELD(value = "restart"),
    Status = TRLC_FIELD(value = "status")
);

// Default policies enum for comparison
TRLC_ENUM(StandardEnum,
    Alpha = TRLC_FIELD(value = 10),
    Beta,   // Should be 11
    Gamma,  // Should be 12
    Delta   // Should be 13
);

TEST(EnumPoliciesTest, CustomIncrementPolicy) {
    // PowerOf2 increment
    EXPECT_EQ(PowerEnum::First.value(), 1);
    EXPECT_EQ(PowerEnum::Second.value(), 2);
    EXPECT_EQ(PowerEnum::Third.value(), 4);
    EXPECT_EQ(PowerEnum::Fourth.value(), 8);
}

TEST(EnumPoliciesTest, CustomPoliciesValues) {
    EXPECT_EQ(PowerEnum::SIZE, 4);
    EXPECT_EQ(PowerEnum::datas[0].value, 1);
    EXPECT_EQ(PowerEnum::datas[1].value, 2);
    EXPECT_EQ(PowerEnum::datas[2].value, 4);
    EXPECT_EQ(PowerEnum::datas[3].value, 8);
}

TEST(EnumPoliciesTest, CustomPoliciesLookup) {
    auto opt1 = PowerEnum::fromValue(1);
    auto opt2 = PowerEnum::fromValue(4);
    auto opt3 = PowerEnum::fromValue(8);
    
    ASSERT_TRUE(opt1.has_value());
    ASSERT_TRUE(opt2.has_value());
    ASSERT_TRUE(opt3.has_value());
    
    EXPECT_EQ(opt1->name(), "First");
    EXPECT_EQ(opt2->name(), "Third");
    EXPECT_EQ(opt3->name(), "Fourth");
}

TEST(EnumPoliciesTest, CaseInsensitiveSearch) {
    auto opt1 = Command::fromValue("start");
    auto opt2 = Command::fromValue("START");
    auto opt3 = Command::fromValue("Start");
    
    ASSERT_TRUE(opt1.has_value());
    ASSERT_TRUE(opt2.has_value());
    ASSERT_TRUE(opt3.has_value());
    
    EXPECT_EQ(opt1->name(), "Start");
    EXPECT_EQ(opt2->name(), "Start");
    EXPECT_EQ(opt3->name(), "Start");
}

TEST(EnumPoliciesTest, CaseInsensitiveAllValues) {
    // All case variations should work
    EXPECT_TRUE(Command::fromValue("stop").has_value());
    EXPECT_TRUE(Command::fromValue("STOP").has_value());
    EXPECT_TRUE(Command::fromValue("Stop").has_value());
    EXPECT_TRUE(Command::fromValue("StOp").has_value());
}

TEST(EnumPoliciesTest, DefaultPoliciesLookup) {
    auto opt = StandardEnum::fromValue(10);
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->name(), "Alpha");
}

TEST(EnumPoliciesTest, DefaultPoliciesIncrement) {
    // Default increment is +1
    EXPECT_EQ(StandardEnum::Alpha.value(), 10);
    EXPECT_EQ(StandardEnum::Beta.value(), 11);
    EXPECT_EQ(StandardEnum::Gamma.value(), 12);
    EXPECT_EQ(StandardEnum::Delta.value(), 13);
}

TEST(EnumPoliciesTest, PowerOf2NotFound) {
    // Values that are not powers of 2 from First
    EXPECT_FALSE(PowerEnum::fromValue(3).has_value());
    EXPECT_FALSE(PowerEnum::fromValue(5).has_value());
    EXPECT_FALSE(PowerEnum::fromValue(16).has_value());
}

TEST(EnumPoliciesTest, CaseInsensitiveNameLookup) {
    // fromName also uses policy CASE_SENSITIVE setting
    auto opt1 = Command::fromName("Start");
    auto opt2 = Command::fromName("start");
    auto opt3 = Command::fromName("START");
    
    EXPECT_TRUE(opt1.has_value());
    EXPECT_TRUE(opt2.has_value());  // Case-insensitive policies apply to names too!
    EXPECT_TRUE(opt3.has_value());
    
    // All should find the same enum
    EXPECT_EQ(opt1->name(), "Start");
    EXPECT_EQ(opt2->name(), "Start");
    EXPECT_EQ(opt3->name(), "Start");
}

} // namespace
