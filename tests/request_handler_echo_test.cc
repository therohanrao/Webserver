#include "gtest/gtest.h"
#include <iostream>
#include <boost/beast.hpp>

#include "request_handler_echo.h"
#include "utils.h"

class EchoHandlerTest : public ::testing::Test
{
protected:
    NginxConfigParser config_parser;
    http::request<http::string_body> req;
    http::response<http::string_body> res;
};

// A test to check if the output of an echo request is a copy of the client request
TEST_F(EchoHandlerTest, EchoRequestTest)
{
    EchoHandler request_handler_echo("/echo", "/echo");
    std::string reply = "";
    req.method(http::verb::get);
    req.target("/echo");
    req.version(11);
    request_handler_echo.handle_request(req, res);

    const std::string body = "GET /echo HTTP/1.1\r\n\r\n";

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    expected_response.set(http::field::content_type, "text/plain");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(res, expected_response);
    EXPECT_TRUE(isEqual);
}