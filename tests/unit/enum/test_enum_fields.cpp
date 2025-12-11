#include <gtest/gtest.h>
#include "trlc/enum.hpp"

namespace {

// Test with integer values and descriptions
TRLC_ENUM(HttpStatus,
    OK = TRLC_FIELD(value = 200, desc = "Request successful"),
    Created = TRLC_FIELD(value = 201, desc = "Resource created"),
    BadRequest = TRLC_FIELD(value = 400, desc = "Invalid request"),
    NotFound = TRLC_FIELD(value = 404, desc = "Resource not found"),
    InternalServerError = TRLC_FIELD(value = 500, desc = "Server error")
);

// Test with string_view values using custom policies
struct StringValuePolicies {
    using ValueType = std::string_view;
    using DescType = std::string_view;
    using ExtraType = std::string_view;
    
    inline static constexpr bool CASE_SENSITIVE{true};
    
    using Increment = trlc::enums::policies::DefaultIncrementPolicy<ValueType>;
    using Search = trlc::enums::policies::LinearSearchPolicy<ValueType, CASE_SENSITIVE>;
    using Comparison = trlc::enums::policies::LexicographicComparisonPolicy<ValueType, CASE_SENSITIVE>;
};

TRLC_ENUM_DEF(LogLevel, StringValuePolicies,
    Debug = TRLC_FIELD(value = "DEBUG", desc = "Detailed debugging information"),
    Info = TRLC_FIELD(value = "INFO", desc = "General informational messages"),
    Warning = TRLC_FIELD(value = "WARN", desc = "Warning messages"),
    Error = TRLC_FIELD(value = "ERROR", desc = "Error messages"),
    Critical = TRLC_FIELD(value = "CRIT", desc = "Critical failure messages")
);

// Test with extra field
TRLC_ENUM(Priority,
    Low = TRLC_FIELD(value = 1, desc = "Low priority", extra = "P3"),
    Medium = TRLC_FIELD(value = 2, desc = "Medium priority", extra = "P2"),
    High = TRLC_FIELD(value = 3, desc = "High priority", extra = "P1"),
    Critical = TRLC_FIELD(value = 4, desc = "Critical priority", extra = "P0")
);

TEST(EnumFieldsTest, IntegerValues) {
    EXPECT_EQ(HttpStatus::OK.value(), 200);
    EXPECT_EQ(HttpStatus::Created.value(), 201);
    EXPECT_EQ(HttpStatus::BadRequest.value(), 400);
    EXPECT_EQ(HttpStatus::NotFound.value(), 404);
    EXPECT_EQ(HttpStatus::InternalServerError.value(), 500);
}

TEST(EnumFieldsTest, StringViewValues) {
    EXPECT_EQ(LogLevel::Debug.value(), "DEBUG");
    EXPECT_EQ(LogLevel::Info.value(), "INFO");
    EXPECT_EQ(LogLevel::Warning.value(), "WARN");
    EXPECT_EQ(LogLevel::Error.value(), "ERROR");
    EXPECT_EQ(LogLevel::Critical.value(), "CRIT");
}

TEST(EnumFieldsTest, Descriptions) {
    EXPECT_EQ(HttpStatus::OK.desc(), "Request successful");
    EXPECT_EQ(HttpStatus::NotFound.desc(), "Resource not found");
    EXPECT_EQ(LogLevel::Debug.desc(), "Detailed debugging information");
    EXPECT_EQ(LogLevel::Error.desc(), "Error messages");
}

TEST(EnumFieldsTest, ExtraField) {
    EXPECT_EQ(Priority::Low.extra(), "P3");
    EXPECT_EQ(Priority::Medium.extra(), "P2");
    EXPECT_EQ(Priority::High.extra(), "P1");
    EXPECT_EQ(Priority::Critical.extra(), "P0");
}

TEST(EnumFieldsTest, Names) {
    EXPECT_EQ(HttpStatus::OK.name(), "OK");
    EXPECT_EQ(HttpStatus::BadRequest.name(), "BadRequest");
    EXPECT_EQ(LogLevel::Debug.name(), "Debug");
    EXPECT_EQ(Priority::High.name(), "High");
}

TEST(EnumFieldsTest, ValuesArray) {
    EXPECT_EQ(HttpStatus::datas[0].value, 200);
    EXPECT_EQ(HttpStatus::datas[4].value, 500);
    EXPECT_EQ(LogLevel::datas[0].value, "DEBUG");
    EXPECT_EQ(LogLevel::datas[4].value, "CRIT");
}

TEST(EnumFieldsTest, DescsArray) {
    EXPECT_EQ(HttpStatus::datas[0].desc, "Request successful");
    EXPECT_EQ(HttpStatus::datas[3].desc, "Resource not found");
    EXPECT_EQ(LogLevel::datas[1].desc, "General informational messages");
}

TEST(EnumFieldsTest, NamesArray) {
    EXPECT_EQ(HttpStatus::datas[0].name, "OK");
    EXPECT_EQ(HttpStatus::datas[3].name, "NotFound");
    EXPECT_EQ(LogLevel::datas[2].name, "Warning");
}

TEST(EnumFieldsTest, ExtrasArray) {
    EXPECT_EQ(Priority::datas[0].extra, "P3");
    EXPECT_EQ(Priority::datas[3].extra, "P0");
}

TEST(EnumFieldsTest, Indices) {
    EXPECT_EQ(HttpStatus::OK.index(), 0);
    EXPECT_EQ(HttpStatus::InternalServerError.index(), 4);
    EXPECT_EQ(LogLevel::Debug.index(), 0);
    EXPECT_EQ(Priority::Critical.index(), 3);
}

TEST(EnumFieldsTest, ConstexprValues) {
    static_assert(HttpStatus::SIZE == 5);
    static_assert(LogLevel::SIZE == 5);
    static_assert(Priority::SIZE == 4);
}

TEST(EnumFieldsTest, MixedFields) {
    // Some fields with desc only
    auto status = HttpStatus::OK;
    EXPECT_EQ(status.value(), 200);
    EXPECT_EQ(status.desc(), "Request successful");
    EXPECT_EQ(status.name(), "OK");
    
    // Fields with all three
    auto prio = Priority::High;
    EXPECT_EQ(prio.value(), 3);
    EXPECT_EQ(prio.desc(), "High priority");
    EXPECT_EQ(prio.extra(), "P1");
}

TEST(EnumFieldsTest, EnumSwap) {
    HttpStatus::Enum status1 = HttpStatus::OK;
    HttpStatus::Enum status2 = HttpStatus::NotFound;
    
    EXPECT_EQ(status1.value(), 200);
    EXPECT_EQ(status2.value(), 404);
    
    // Swap values
    std::swap(status1, status2);
    
    EXPECT_EQ(status1.value(), 404);
    EXPECT_EQ(status2.value(), 200);
    EXPECT_EQ(status1.name(), "NotFound");
    EXPECT_EQ(status2.name(), "OK");
}

TEST(EnumFieldsTest, EnumMove) {
    Priority::Enum prio1 = Priority::High;
    EXPECT_EQ(prio1.extra(), "P1");
    
    // Move assignment
    Priority::Enum prio2 = std::move(prio1);
    EXPECT_EQ(prio2.extra(), "P1");
    EXPECT_EQ(prio2.name(), "High");
    
    // Can still assign to moved-from object
    prio1 = Priority::Low;
    EXPECT_EQ(prio1.extra(), "P3");
}

} // namespace
