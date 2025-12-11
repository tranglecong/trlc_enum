#include <gtest/gtest.h>
#include "trlc/enum.hpp"
#include <algorithm>
#include <string>
#include <sstream>

namespace {

// Complete HTTP status codes implementation
TRLC_ENUM(HttpStatus,
    // 1xx Informational
    Continue = TRLC_FIELD(value = 100, desc = "Continue"),
    SwitchingProtocols = TRLC_FIELD(value = 101, desc = "Switching Protocols"),
    
    // 2xx Success
    OK = TRLC_FIELD(value = 200, desc = "OK"),
    Created = TRLC_FIELD(value = 201, desc = "Created"),
    Accepted = TRLC_FIELD(value = 202, desc = "Accepted"),
    NoContent = TRLC_FIELD(value = 204, desc = "No Content"),
    
    // 3xx Redirection
    MovedPermanently = TRLC_FIELD(value = 301, desc = "Moved Permanently"),
    Found = TRLC_FIELD(value = 302, desc = "Found"),
    SeeOther = TRLC_FIELD(value = 303, desc = "See Other"),
    NotModified = TRLC_FIELD(value = 304, desc = "Not Modified"),
    TemporaryRedirect = TRLC_FIELD(value = 307, desc = "Temporary Redirect"),
    PermanentRedirect = TRLC_FIELD(value = 308, desc = "Permanent Redirect"),
    
    // 4xx Client Errors
    BadRequest = TRLC_FIELD(value = 400, desc = "Bad Request"),
    Unauthorized = TRLC_FIELD(value = 401, desc = "Unauthorized"),
    Forbidden = TRLC_FIELD(value = 403, desc = "Forbidden"),
    NotFound = TRLC_FIELD(value = 404, desc = "Not Found"),
    MethodNotAllowed = TRLC_FIELD(value = 405, desc = "Method Not Allowed"),
    NotAcceptable = TRLC_FIELD(value = 406, desc = "Not Acceptable"),
    RequestTimeout = TRLC_FIELD(value = 408, desc = "Request Timeout"),
    Conflict = TRLC_FIELD(value = 409, desc = "Conflict"),
    Gone = TRLC_FIELD(value = 410, desc = "Gone"),
    TooManyRequests = TRLC_FIELD(value = 429, desc = "Too Many Requests"),
    
    // 5xx Server Errors
    InternalServerError = TRLC_FIELD(value = 500, desc = "Internal Server Error"),
    NotImplemented = TRLC_FIELD(value = 501, desc = "Not Implemented"),
    BadGateway = TRLC_FIELD(value = 502, desc = "Bad Gateway"),
    ServiceUnavailable = TRLC_FIELD(value = 503, desc = "Service Unavailable"),
    GatewayTimeout = TRLC_FIELD(value = 504, desc = "Gateway Timeout")
);

TEST(HttpStatusTest, BasicStatusCodes) {
    EXPECT_EQ(HttpStatus::OK.value(), 200);
    EXPECT_EQ(HttpStatus::NotFound.value(), 404);
    EXPECT_EQ(HttpStatus::InternalServerError.value(), 500);
}

TEST(HttpStatusTest, StatusDescriptions) {
    EXPECT_EQ(HttpStatus::OK.desc(), "OK");
    EXPECT_EQ(HttpStatus::BadRequest.desc(), "Bad Request");
    EXPECT_EQ(HttpStatus::ServiceUnavailable.desc(), "Service Unavailable");
}

TEST(HttpStatusTest, LookupByCode) {
    auto status = HttpStatus::fromValue(404);
    ASSERT_TRUE(status.has_value());
    EXPECT_EQ(status->name(), "NotFound");
    EXPECT_EQ(status->desc(), "Not Found");
}

TEST(HttpStatusTest, LookupUnknownCode) {
    auto status = HttpStatus::fromValue(999);
    EXPECT_FALSE(status.has_value());
}

TEST(HttpStatusTest, IsSuccessRange) {
    auto is_success = [](int code) {
        auto status = HttpStatus::fromValue(code);
        return status.has_value() && status->value() >= 200 && status->value() < 300;
    };
    
    EXPECT_TRUE(is_success(200));
    EXPECT_TRUE(is_success(201));
    EXPECT_TRUE(is_success(204));
    EXPECT_FALSE(is_success(404));
    EXPECT_FALSE(is_success(500));
}

TEST(HttpStatusTest, IsClientErrorRange) {
    auto is_client_error = [](int code) {
        auto status = HttpStatus::fromValue(code);
        return status.has_value() && status->value() >= 400 && status->value() < 500;
    };
    
    EXPECT_TRUE(is_client_error(400));
    EXPECT_TRUE(is_client_error(404));
    EXPECT_TRUE(is_client_error(429));
    EXPECT_FALSE(is_client_error(200));
    EXPECT_FALSE(is_client_error(500));
}

TEST(HttpStatusTest, IsServerErrorRange) {
    auto is_server_error = [](int code) {
        auto status = HttpStatus::fromValue(code);
        return status.has_value() && status->value() >= 500 && status->value() < 600;
    };
    
    EXPECT_TRUE(is_server_error(500));
    EXPECT_TRUE(is_server_error(502));
    EXPECT_TRUE(is_server_error(504));
    EXPECT_FALSE(is_server_error(200));
    EXPECT_FALSE(is_server_error(404));
}

TEST(HttpStatusTest, FormatResponse) {
    auto format_response = [](int code) -> std::string {
        auto status = HttpStatus::fromValue(code);
        if (!status.has_value()) {
            return "Unknown Status";
        }
        
        std::ostringstream oss;
        oss << status->value() << " " << status->desc();
        return oss.str();
    };
    
    EXPECT_EQ(format_response(200), "200 OK");
    EXPECT_EQ(format_response(404), "404 Not Found");
    EXPECT_EQ(format_response(500), "500 Internal Server Error");
    EXPECT_EQ(format_response(999), "Unknown Status");
}

TEST(HttpStatusTest, IterateAllStatuses) {
    std::size_t count = 0;
    
    for (HttpStatus::Enum status : HttpStatus::all()) {
        EXPECT_FALSE(status.name().empty());
        EXPECT_FALSE(status.desc().empty());
        EXPECT_GT(status.value(), 0);
        count++;
    }
    
    EXPECT_EQ(count, HttpStatus::SIZE);
}

TEST(HttpStatusTest, FindRedirects) {
    std::vector<HttpStatus::Enum> redirects;
    
    for (HttpStatus::Enum status : HttpStatus::all()) {
        if (status.value() >= 300 && status.value() < 400) {
            redirects.push_back(status);
        }
    }
    
    EXPECT_FALSE(redirects.empty());
    EXPECT_TRUE(std::any_of(redirects.begin(), redirects.end(), [](auto s) {
        return s.value() == 301;
    }));
}

TEST(HttpStatusTest, CategoryCount) {
    int informational = 0, success = 0, redirect = 0, client_error = 0, server_error = 0;
    
    for (HttpStatus::Enum status : HttpStatus::all()) {
        int code = status.value();
        if (code >= 100 && code < 200) informational++;
        else if (code >= 200 && code < 300) success++;
        else if (code >= 300 && code < 400) redirect++;
        else if (code >= 400 && code < 500) client_error++;
        else if (code >= 500 && code < 600) server_error++;
    }
    
    EXPECT_GT(informational, 0);
    EXPECT_GT(success, 0);
    EXPECT_GT(redirect, 0);
    EXPECT_GT(client_error, 0);
    EXPECT_GT(server_error, 0);
}

TEST(HttpStatusTest, CompareStatuses) {
    EXPECT_TRUE(HttpStatus::OK == HttpStatus::OK);
    EXPECT_FALSE(HttpStatus::OK == HttpStatus::NotFound);
    EXPECT_TRUE(HttpStatus::OK != HttpStatus::NotFound);
}

} // namespace
