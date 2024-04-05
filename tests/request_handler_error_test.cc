#include "gtest/gtest.h"
#include <iostream>
#include <boost/beast.hpp>

#include "request_handler_error.h"
#include "utils.h"

class ErrorHandlerTest : public ::testing::Test
{
protected:
    http::request<http::string_body> req;
    http::response<http::string_body> response;
};

// A test to check if the output of an echo request is a copy of the client request
TEST_F(ErrorHandlerTest, ErrorRequestTest)
{
    ErrorHandler request_handler_error("/", "/asdf");
    std::string reply = "";
    char input[1024] = "GET /asdf HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/asdf");
    req.version(11);
    request_handler_error.handle_request(req, response);

    std::string body = std::string(input);

    http::response<http::string_body> expected_response;
    std::string file_not_found = "File not found.\r\n";
    expected_response.version(11);
    expected_response.result(http::status::not_found);
    expected_response.set(http::field::content_type, "text/plain");
    expected_response.set(http::field::content_length, std::to_string(file_not_found.length()));
    expected_response.body() = file_not_found;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}