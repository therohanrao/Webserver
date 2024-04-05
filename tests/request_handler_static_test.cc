#include "gtest/gtest.h"
#include <iostream>
#include <boost/beast.hpp>

#include "config_parser.h"
#include "request_handler_echo.h"
#include "request_handler_static.h"
#include "utils.h"

class StaticHandlerTest : public ::testing::Test
{
protected:
    NginxConfigParser config_parser;
    char c;
    std::ifstream f;
    http::request<http::string_body> req;
    http::response<http::string_body> response;
};

// A test to check if the output of a text file request is what it should be (an
// OK and echoing of the text file)
TEST_F(StaticHandlerTest, TxtHandlerRequestTest)
{
    StaticHandler request_handler_static("/static", "/static/static.txt", "./folder1");
    std::string reply = "";
    char input[1024] = "GET /static/static.txt HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/static.txt");
    req.version(11);
    request_handler_static.handle_request(req, response);

    f.open("./folder1/static.txt");

    std::string body;
    while (f.get(c))
        body += c;
    f.close();

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    if (req.has_content_length())
    {
        expected_response.set(http::field::content_length, req[http::field::content_length]);
    }
    expected_response.set(http::field::content_type, "text/plain");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}

// A test to check if the output of a text file request in a different directory is what it should be (an
// OK and echoing of the text file)
TEST_F(StaticHandlerTest, Txt2HandlerRequestTest)
{
    StaticHandler request_handler_static("/static2", "/static2/staticv2.txt", "./folder2");
    std::string reply = "";
    char input[1024] = "GET /static2/staticv2.txt HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/static.txt");
    req.version(11);
    request_handler_static.handle_request(req, response);

    f.open("./folder2/staticv2.txt");

    std::string body;
    while (f.get(c))
        body += c;
    f.close();

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    if (req.has_content_length())
    {
        expected_response.set(http::field::content_length, req[http::field::content_length]);
    }
    expected_response.set(http::field::content_type, "text/plain");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}

// A test to check if the output of a html file request is what it should be (an
// OK and echoing of the html file)
TEST_F(StaticHandlerTest, HtmlHandlerRequestTest)
{
    StaticHandler request_handler_static("/static", "/static/index.html", "./folder1");
    std::string reply = "";
    char input[1024] = "GET /static/index.html HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/index.html");
    req.version(11);
    request_handler_static.handle_request(req, response);

    f.open("./folder1/index.html");

    std::string body;
    while (f.get(c))
        body += c;
    f.close();

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    if (req.has_content_length())
    {
        expected_response.set(http::field::content_length, req[http::field::content_length]);
    }
    expected_response.set(http::field::content_type, "text/html");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}

// A test to check if the output of a dat file request is what it should be (an
// OK and echoing of the dat file)
TEST_F(StaticHandlerTest, DatHandlerRequestTest)
{
    StaticHandler request_handler_static("/static", "/static/data.dat", "./folder1");
    std::string reply = "";
    char input[1024] = "GET /static/data.dat HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/data.dat");
    req.version(11);
    request_handler_static.handle_request(req, response);

    f.open("./folder1/data.dat");

    std::string body;
    while (f.get(c))
        body += c;
    f.close();

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    if (req.has_content_length())
    {
        expected_response.set(http::field::content_length, req[http::field::content_length]);
    }
    expected_response.set(http::field::content_type, "text/plain");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}

// A test to check if the output of a pdf file request is what it should be (an
// OK and echoing of the pdf file)
TEST_F(StaticHandlerTest, PdfHandlerRequestTest)
{
    StaticHandler request_handler_static("/static", "/static/assign4pdf.pdf", "./folder1");
    std::string reply = "";
    char input[1024] = "GET /static/assign4pdf.pdf HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/assign4pdf.pdf");
    req.version(11);
    request_handler_static.handle_request(req, response);

    f.open("./folder1/assign4pdf.pdf");

    std::string body;
    while (f.get(c))
        body += c;
    f.close();

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    if (req.has_content_length())
    {
        expected_response.set(http::field::content_length, req[http::field::content_length]);
    }
    expected_response.set(http::field::content_type, "application/pdf");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}

// A test to check if the output of a zip folder request is what it should be (an
// OK and echoing of the zip folder)
TEST_F(StaticHandlerTest, ZipHandlerRequestTest)
{
    StaticHandler request_handler_static("/static", "/static/zipped.zip", "./folder1");
    std::string reply = "";
    char input[1024] = "GET /static/zipped.zip HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/zipped.zip");
    req.version(11);
    request_handler_static.handle_request(req, response);

    f.open("./folder1/zipped.zip");

    std::string body;
    while (f.get(c))
        body += c;
    f.close();

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    if (req.has_content_length())
    {
        expected_response.set(http::field::content_length, req[http::field::content_length]);
    }
    expected_response.set(http::field::content_type, "application/zip");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}

// A test to check if the output of a gif file request is what it should be (an
// OK and echoing of the gif file)
TEST_F(StaticHandlerTest, GifHandlerRequestTest)
{
    StaticHandler request_handler_static("/static", "/static/gif.gif", "./folder1");
    std::string reply = "";
    char input[1024] = "GET /static/gif.gif HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/gif.gif");
    req.version(11);
    request_handler_static.handle_request(req, response);

    f.open("./folder1/gif.gif");

    std::string body;
    while (f.get(c))
        body += c;
    f.close();

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    if (req.has_content_length())
    {
        expected_response.set(http::field::content_length, req[http::field::content_length]);
    }
    expected_response.set(http::field::content_type, "image/gif");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}

// A test to check if the output of a jpeg file request is what it should be (an
// OK and echoing of the jpeg file)
TEST_F(StaticHandlerTest, JpegHandlerRequestTest)
{
    StaticHandler request_handler_static("/static", "/static/server.jpg", "./folder1");
    std::string reply = "";
    char input[1024] = "GET /static/server.jpg HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/server.jpg");
    req.version(11);
    request_handler_static.handle_request(req, response);

    f.open("./folder1/server.jpg");

    std::string body;
    while (f.get(c))
        body += c;
    f.close();

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    if (req.has_content_length())
    {
        expected_response.set(http::field::content_length, req[http::field::content_length]);
    }
    expected_response.set(http::field::content_type, "image/jpeg");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}

// A test to check if the output of a png file request is what it should be (an
// OK and echoing of the png file)
TEST_F(StaticHandlerTest, PngHandlerRequestTest)
{
    StaticHandler request_handler_static("/static", "/static/server.png", "./folder1");
    std::string reply = "";
    char input[1024] = "GET /static/server.png HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/server.png");
    req.version(11);
    request_handler_static.handle_request(req, response);

    f.open("./folder1/server.png");

    std::string body;
    while (f.get(c))
        body += c;
    f.close();

    http::response<http::string_body> expected_response;
    expected_response.version(11);
    expected_response.result(http::status::ok);
    expected_response.reason("OK");
    if (req.has_content_length())
    {
        expected_response.set(http::field::content_length, req[http::field::content_length]);
    }
    expected_response.set(http::field::content_type, "image/png");
    expected_response.body() = body;
    expected_response.prepare_payload();

    bool isEqual = checkResponseEqual(response, expected_response);
    EXPECT_TRUE(isEqual);
}

// A test that should output an error of not finding a file
TEST_F(StaticHandlerTest, NoFileHandlerRequestTest)
{
    StaticHandler request_handler_static("/static", "/static/none.txt", "./folder1");
    std::string reply = "";
    char input[1024] = "GET /static/none.txt HTTP/1.1\r\nHost: "
                       "www.example.com\r\nConnection: close\r\n\r\n";
    req.content_length(std::strlen(input));
    req.body() = input;
    req.method(http::verb::get);
    req.target("/static/none.txt");
    req.version(11);
    request_handler_static.handle_request(req, response);

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