#include <gtest/gtest.h>
#include "trlc/enum.hpp"
#include <algorithm>
#include <string>
#include <map>
#include <vector>

namespace {

// Application error code hierarchy with categories
TRLC_ENUM(ErrorCode,
    // Success
    Success = TRLC_FIELD(value = 0, desc = "Operation completed successfully", extra = "INFO"),
    
    // File Errors (1000-1999)
    FileNotFound = TRLC_FIELD(value = 1001, desc = "File does not exist", extra = "FILE"),
    FilePermissionDenied = TRLC_FIELD(value = 1002, desc = "Permission denied", extra = "FILE"),
    FileAlreadyExists = TRLC_FIELD(value = 1003, desc = "File already exists", extra = "FILE"),
    FileTooLarge = TRLC_FIELD(value = 1004, desc = "File size exceeds limit", extra = "FILE"),
    FileCorrupted = TRLC_FIELD(value = 1005, desc = "File is corrupted", extra = "FILE"),
    
    // Network Errors (2000-2999)
    NetworkTimeout = TRLC_FIELD(value = 2001, desc = "Network request timed out", extra = "NETWORK"),
    NetworkUnreachable = TRLC_FIELD(value = 2002, desc = "Network is unreachable", extra = "NETWORK"),
    ConnectionRefused = TRLC_FIELD(value = 2003, desc = "Connection refused by server", extra = "NETWORK"),
    ConnectionLost = TRLC_FIELD(value = 2004, desc = "Connection lost during transfer", extra = "NETWORK"),
    
    // Database Errors (3000-3999)
    DatabaseConnectionFailed = TRLC_FIELD(value = 3001, desc = "Cannot connect to database", extra = "DATABASE"),
    DatabaseQueryFailed = TRLC_FIELD(value = 3002, desc = "Query execution failed", extra = "DATABASE"),
    DatabaseConstraintViolation = TRLC_FIELD(value = 3003, desc = "Constraint violation", extra = "DATABASE"),
    DatabaseDeadlock = TRLC_FIELD(value = 3004, desc = "Deadlock detected", extra = "DATABASE"),
    
    // Authentication Errors (4000-4999)
    AuthInvalidCredentials = TRLC_FIELD(value = 4001, desc = "Invalid username or password", extra = "AUTH"),
    AuthTokenExpired = TRLC_FIELD(value = 4002, desc = "Authentication token expired", extra = "AUTH"),
    AuthInsufficientPermissions = TRLC_FIELD(value = 4003, desc = "Insufficient permissions", extra = "AUTH"),
    AuthAccountLocked = TRLC_FIELD(value = 4004, desc = "Account is locked", extra = "AUTH"),
    
    // Validation Errors (5000-5999)
    ValidationInvalidInput = TRLC_FIELD(value = 5001, desc = "Input validation failed", extra = "VALIDATION"),
    ValidationMissingField = TRLC_FIELD(value = 5002, desc = "Required field is missing", extra = "VALIDATION"),
    ValidationInvalidFormat = TRLC_FIELD(value = 5003, desc = "Invalid data format", extra = "VALIDATION"),
    ValidationOutOfRange = TRLC_FIELD(value = 5004, desc = "Value out of acceptable range", extra = "VALIDATION"),
    
    // System Errors (9000-9999)
    SystemOutOfMemory = TRLC_FIELD(value = 9001, desc = "Out of memory", extra = "SYSTEM"),
    SystemResourceExhausted = TRLC_FIELD(value = 9002, desc = "System resources exhausted", extra = "SYSTEM"),
    SystemInternalError = TRLC_FIELD(value = 9003, desc = "Internal system error", extra = "SYSTEM"),
    SystemNotSupported = TRLC_FIELD(value = 9004, desc = "Operation not supported", extra = "SYSTEM")
);

TEST(ErrorCodeTest, BasicErrorCodes) {
    EXPECT_EQ(ErrorCode::Success.value(), 0);
    EXPECT_EQ(ErrorCode::FileNotFound.value(), 1001);
    EXPECT_EQ(ErrorCode::NetworkTimeout.value(), 2001);
    EXPECT_EQ(ErrorCode::DatabaseConnectionFailed.value(), 3001);
}

TEST(ErrorCodeTest, ErrorDescriptions) {
    EXPECT_EQ(ErrorCode::Success.desc(), "Operation completed successfully");
    EXPECT_EQ(ErrorCode::FileNotFound.desc(), "File does not exist");
    EXPECT_EQ(ErrorCode::AuthInvalidCredentials.desc(), "Invalid username or password");
}

TEST(ErrorCodeTest, ErrorCategories) {
    EXPECT_EQ(ErrorCode::FileNotFound.extra(), "FILE");
    EXPECT_EQ(ErrorCode::NetworkTimeout.extra(), "NETWORK");
    EXPECT_EQ(ErrorCode::DatabaseConnectionFailed.extra(), "DATABASE");
    EXPECT_EQ(ErrorCode::AuthInvalidCredentials.extra(), "AUTH");
    EXPECT_EQ(ErrorCode::ValidationInvalidInput.extra(), "VALIDATION");
    EXPECT_EQ(ErrorCode::SystemOutOfMemory.extra(), "SYSTEM");
}

TEST(ErrorCodeTest, LookupByCode) {
    auto error = ErrorCode::fromValue(1001);
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error->name(), "FileNotFound");
    EXPECT_EQ(error->desc(), "File does not exist");
    EXPECT_EQ(error->extra(), "FILE");
}

TEST(ErrorCodeTest, LookupUnknownCode) {
    auto error = ErrorCode::fromValue(9999);
    EXPECT_FALSE(error.has_value());
}

TEST(ErrorCodeTest, IsSuccess) {
    auto is_success = [](int code) {
        auto error = ErrorCode::fromValue(code);
        return error.has_value() && error->value() == 0;
    };
    
    EXPECT_TRUE(is_success(0));
    EXPECT_FALSE(is_success(1001));
    EXPECT_FALSE(is_success(9001));
}

TEST(ErrorCodeTest, GetErrorCategory) {
    auto get_category = [](int code) -> std::string {
        auto error = ErrorCode::fromValue(code);
        if (!error.has_value()) {
            return "UNKNOWN";
        }
        return std::string(error->extra());
    };
    
    EXPECT_EQ(get_category(1001), "FILE");
    EXPECT_EQ(get_category(2001), "NETWORK");
    EXPECT_EQ(get_category(3001), "DATABASE");
    EXPECT_EQ(get_category(4001), "AUTH");
    EXPECT_EQ(get_category(5001), "VALIDATION");
    EXPECT_EQ(get_category(9001), "SYSTEM");
}

TEST(ErrorCodeTest, FilterByCategory) {
    std::vector<ErrorCode::Enum> file_errors;
    
    for (ErrorCode::Enum error : ErrorCode::all()) {
        if (error.extra() == "FILE") {
            file_errors.push_back(error);
        }
    }
    
    EXPECT_EQ(file_errors.size(), 5);
    EXPECT_TRUE(std::any_of(file_errors.begin(), file_errors.end(), [](auto e) {
        return e.value() == 1001;
    }));
}

TEST(ErrorCodeTest, CountErrorsByCategory) {
    std::map<std::string, int> category_counts;
    
    for (ErrorCode::Enum error : ErrorCode::all()) {
        std::string category(error.extra());
        category_counts[category]++;
    }
    
    EXPECT_GT(category_counts["FILE"], 0);
    EXPECT_GT(category_counts["NETWORK"], 0);
    EXPECT_GT(category_counts["DATABASE"], 0);
    EXPECT_GT(category_counts["AUTH"], 0);
    EXPECT_GT(category_counts["VALIDATION"], 0);
    EXPECT_GT(category_counts["SYSTEM"], 0);
}

TEST(ErrorCodeTest, FormatErrorMessage) {
    auto format_error = [](int code) -> std::string {
        auto error = ErrorCode::fromValue(code);
        if (!error.has_value()) {
            return "Unknown Error";
        }
        
        std::string msg = "[";
        msg += error->extra();
        msg += "] Error ";
        msg += std::to_string(error->value());
        msg += ": ";
        msg += error->desc();
        return msg;
    };
    
    EXPECT_EQ(format_error(1001), "[FILE] Error 1001: File does not exist");
    EXPECT_EQ(format_error(2001), "[NETWORK] Error 2001: Network request timed out");
    EXPECT_EQ(format_error(9999), "Unknown Error");
}

TEST(ErrorCodeTest, IsRetryableError) {
    auto is_retryable = [](int code) {
        auto error = ErrorCode::fromValue(code);
        if (!error.has_value()) return false;
        
        std::string category(error->extra());
        return category == "NETWORK" || category == "DATABASE";
    };
    
    EXPECT_TRUE(is_retryable(2001));  // Network timeout
    EXPECT_TRUE(is_retryable(3004));  // Database deadlock
    EXPECT_FALSE(is_retryable(1001)); // File not found
    EXPECT_FALSE(is_retryable(4001)); // Auth error
}

TEST(ErrorCodeTest, GetErrorSeverity) {
    auto get_severity = [](int code) -> std::string {
        auto error = ErrorCode::fromValue(code);
        if (!error.has_value()) return "UNKNOWN";
        
        if (error->value() == 0) return "INFO";
        if (error->value() >= 9000) return "CRITICAL";
        if (error->value() >= 4000) return "ERROR";
        return "WARNING";
    };
    
    EXPECT_EQ(get_severity(0), "INFO");
    EXPECT_EQ(get_severity(1001), "WARNING");
    EXPECT_EQ(get_severity(4001), "ERROR");
    EXPECT_EQ(get_severity(9001), "CRITICAL");
}

TEST(ErrorCodeTest, ErrorRangeCheck) {
    auto is_file_error = [](int code) {
        return code >= 1000 && code < 2000;
    };
    
    auto is_network_error = [](int code) {
        return code >= 2000 && code < 3000;
    };
    
    EXPECT_TRUE(is_file_error(1001));
    EXPECT_FALSE(is_file_error(2001));
    EXPECT_TRUE(is_network_error(2001));
    EXPECT_FALSE(is_network_error(1001));
}

TEST(ErrorCodeTest, CompareErrors) {
    EXPECT_TRUE(ErrorCode::Success == ErrorCode::Success);
    EXPECT_FALSE(ErrorCode::FileNotFound == ErrorCode::NetworkTimeout);
    EXPECT_TRUE(ErrorCode::FileNotFound != ErrorCode::NetworkTimeout);
}

TEST(ErrorCodeTest, AllErrorsHaveCategory) {
    for (ErrorCode::Enum error : ErrorCode::all()) {
        EXPECT_FALSE(error.extra().empty()) << "Error " << error.value() << " has no category";
    }
}

} // namespace
