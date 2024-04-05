#include "gtest/gtest.h"
#include <iostream>
#include <boost/beast.hpp>

#include "config_parser.h"
#include "request_handler_health.h"

bool checkResponseEqual(const http::response<http::string_body> &response1, const http::response<http::string_body> &response2)
{
    if (response1.result() != response2.result())
    {
        return false;
    }

    if (response1.version() != response2.version())
    {
        return false;
    }

    if (response1.body() != response2.body())
    {
        return false;
    }

    return true;
}

// Createe our test fixture class that can used for all tests
class HealthHandlerTest : public ::testing::Test
{
    protected:
        NginxConfigParser config_parser;
        http::request<http::string_body> req;
        http::response<http::string_body> response;
};

TEST_F(HealthHandlerTest, HealthRequestTest)
{
    HealthHandler request_handler_health("/health", "/health");
    char input[1024] = "GET /health HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";

    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/health");
    req.version(11);

    EXPECT_TRUE(request_handler_health.handle_request(req, response));
    
    const std::string body = "OK\r\n";
    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    expected_response.set(http::field::content_length, req[http::field::content_length]);
    expected_response.set(http::field::content_type, "text/plain");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}