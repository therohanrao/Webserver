#include "gtest/gtest.h"
#include <iostream>
#include <boost/beast.hpp>

#include "config_parser.h"
#include "request_handler_sleep.h"
#include "utils.h"

// Createe our test fixture class that can used for all tests
class SleepHandlerTest : public ::testing::Test
{
    protected:
        NginxConfigParser config_parser;
        http::request<http::string_body> req;
        http::response<http::string_body> response;
};

TEST_F(SleepHandlerTest, SleepRequestTest)
{
    SleepHandler request_handler_sleep("/sleep", "/sleep");
    std::string reply = "";
    char input[1024] = "GET /sleep HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";

    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/sleep");
    req.version(11);

    EXPECT_TRUE(request_handler_sleep.handle_request(req, response));
    
    const std::string body = "Slept for 5 seconds.\r\n";
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