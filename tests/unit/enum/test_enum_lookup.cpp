#include <gtest/gtest.h>
#include "trlc/enum.hpp"
#include <string_view>

namespace {

// Sequential integer values for testing
TRLC_ENUM(Status,
    Idle = TRLC_FIELD(value = 0, desc = "System idle"),
    Running = TRLC_FIELD(value = 1, desc = "System running"),
    Paused = TRLC_FIELD(value = 2, desc = "System paused"),
    Stopped = TRLC_FIELD(value = 3, desc = "System stopped")
);

// Non-sequential values with gaps
TRLC_ENUM(ErrorCode,
    Success = TRLC_FIELD(value = 0, desc = "No error"),
    MinorError = TRLC_FIELD(value = 2, desc = "Minor error"),
    MajorError = TRLC_FIELD(value = 5, desc = "Major error"),
    CriticalError = TRLC_FIELD(value = 22, desc = "Critical error")
);

// String values for testing
struct StringValuePolicies {
    using ValueType = std::string_view;
    using DescType = std::string_view;
    using ExtraType = std::string_view;
    
    inline static constexpr bool CASE_SENSITIVE{true};
    
    using Increment = trlc::enums::policies::DefaultIncrementPolicy<ValueType>;
    using Search = trlc::enums::policies::LinearSearchPolicy<ValueType, CASE_SENSITIVE>;
    using Comparison = trlc::enums::policies::LexicographicComparisonPolicy<ValueType, CASE_SENSITIVE>;
};

TRLC_ENUM_DEF(FileType, StringValuePolicies,
    Text = TRLC_FIELD(value = "txt", desc = "Text file"),
    Json = TRLC_FIELD(value = "json", desc = "JSON file"),
    Xml = TRLC_FIELD(value = "xml", desc = "XML file"),
    Binary = TRLC_FIELD(value = "bin", desc = "Binary file")
);

TEST(EnumLookupTest, FromValueSuccess) {
    auto opt = Status::fromValue(1);
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->name(), "Running");
    EXPECT_EQ(opt->value(), 1);
}

TEST(EnumLookupTest, FromValueNotFound) {
    auto opt = Status::fromValue(99);
    EXPECT_FALSE(opt.has_value());
}

TEST(EnumLookupTest, FromValueFirst) {
    auto opt = Status::fromValue(0);
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->name(), "Idle");
}

TEST(EnumLookupTest, FromValueLast) {
    auto opt = Status::fromValue(3);
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->name(), "Stopped");
}

TEST(EnumLookupTest, FromValueNonSequential) {
    auto opt1 = ErrorCode::fromValue(0);
    auto opt2 = ErrorCode::fromValue(2);
    auto opt3 = ErrorCode::fromValue(22);
    
    ASSERT_TRUE(opt1.has_value());
    ASSERT_TRUE(opt2.has_value());
    ASSERT_TRUE(opt3.has_value());
    
    EXPECT_EQ(opt1->name(), "Success");
    EXPECT_EQ(opt2->name(), "MinorError");
    EXPECT_EQ(opt3->name(), "CriticalError");
}

TEST(EnumLookupTest, FromValueGaps) {
    // Values in gaps should not be found
    EXPECT_FALSE(ErrorCode::fromValue(1).has_value());
    EXPECT_FALSE(ErrorCode::fromValue(3).has_value());
    EXPECT_FALSE(ErrorCode::fromValue(10).has_value());
}

TEST(EnumLookupTest, FromNameSuccess) {
    auto opt = Status::fromName("Running");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->value(), 1);
    EXPECT_EQ(opt->name(), "Running");
}

TEST(EnumLookupTest, FromNameNotFound) {
    auto opt = Status::fromName("Unknown");
    EXPECT_FALSE(opt.has_value());
}

TEST(EnumLookupTest, FromNameCaseSensitive) {
    auto opt1 = Status::fromName("Running");
    auto opt2 = Status::fromName("running");
    auto opt3 = Status::fromName("RUNNING");
    
    EXPECT_TRUE(opt1.has_value());
    EXPECT_FALSE(opt2.has_value());
    EXPECT_FALSE(opt3.has_value());
}

TEST(EnumLookupTest, FromNameFirst) {
    auto opt = Status::fromName("Idle");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->value(), 0);
}

TEST(EnumLookupTest, FromNameLast) {
    auto opt = Status::fromName("Stopped");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->value(), 3);
}

TEST(EnumLookupTest, FromNameAllValues) {
    for (std::size_t i = 0; i < Status::SIZE; ++i) {
        auto opt = Status::fromName(Status::datas[i].name);
        ASSERT_TRUE(opt.has_value()) << "Failed for name: " << Status::datas[i].name;
        EXPECT_EQ(opt->index(), i);
    }
}

TEST(EnumLookupTest, FromValueAllValues) {
    for (std::size_t i = 0; i < Status::SIZE; ++i) {
        auto opt = Status::fromValue(Status::datas[i].value);
        ASSERT_TRUE(opt.has_value()) << "Failed for value: " << Status::datas[i].value;
        EXPECT_EQ(opt->index(), i);
    }
}

TEST(EnumLookupTest, LookupAndCompare) {
    auto opt = Status::fromValue(1);
    ASSERT_TRUE(opt.has_value());
    EXPECT_TRUE(*opt == Status::Running);
    EXPECT_FALSE(*opt == Status::Idle);
}

TEST(EnumLookupTest, ChainedLookup) {
    auto opt1 = Status::fromName("Paused");
    ASSERT_TRUE(opt1.has_value());
    
    auto opt2 = Status::fromValue(opt1->value());
    ASSERT_TRUE(opt2.has_value());
    EXPECT_TRUE(*opt1 == *opt2);
}

TEST(EnumLookupTest, StringValueLookup) {
    auto opt1 = FileType::fromValue("json");
    ASSERT_TRUE(opt1.has_value());
    EXPECT_EQ(opt1->name(), "Json");
    
    auto opt2 = FileType::fromValue("unknown");
    EXPECT_FALSE(opt2.has_value());
}

TEST(EnumLookupTest, StringNameLookup) {
    auto opt = FileType::fromName("Text");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->value(), "txt");
}

TEST(EnumLookupTest, FromNameStringView) {
    std::string_view name = "Running";
    auto opt = Status::fromName(name);
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->value(), 1);
}

TEST(EnumLookupTest, FromNameStdString) {
    std::string name = "Paused";
    auto opt = Status::fromName(name);
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->value(), 2);
}

} // namespace
