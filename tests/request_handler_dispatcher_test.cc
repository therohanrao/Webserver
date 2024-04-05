#include "gtest/gtest.h"
#include <iostream>
#include <map>

#include "request_handler_dispatcher.h"
#include "request_handler.h"
#include "request_handler_echo.h"
#include "request_handler_static.h"
#include "request_handler_error.h"
#include "request_handler_factory.h"


class RequestHandlerDispatcherTest : public ::testing::Test
{
protected:
    std::string NO_MATCH = "/";
    NginxConfigParser parser;
    NginxConfig out_config;
};


TEST_F(RequestHandlerDispatcherTest, TestMatchExists)
{
    bool success = parser.Parse("check_mapping_config", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "/static/static.txt";

    std::string match_result = dispatcher.match(routes, request_url);
    EXPECT_TRUE(match_result == "/static");
}

TEST_F(RequestHandlerDispatcherTest, TestMatchMultiDirectoryExists) {
    bool success = parser.Parse("check_mapping_config", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "/static/hello/static.txt";

    std::string match_result = dispatcher.match(routes, request_url);
    std::cout << "MATCH RESULT: " << match_result << std::endl;
    EXPECT_TRUE(match_result == "/static/hello");
}

TEST_F(RequestHandlerDispatcherTest, TestMatchPrefixExists) {
    bool success = parser.Parse("check_mapping_config2", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "/static/hello/static.txt";

    std::string match_result = dispatcher.match(routes, request_url);
    EXPECT_TRUE(match_result == "/static");
}

TEST_F(RequestHandlerDispatcherTest, TestMatchNotExists) {
    bool success = parser.Parse("check_mapping_config", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "/foo/hello/static.txt";

    std::string match_result = dispatcher.match(routes, request_url);
    EXPECT_TRUE(match_result == NO_MATCH);
}

TEST_F(RequestHandlerDispatcherTest, TestMatchEmptyPath) {
    bool success = parser.Parse("check_mapping_config", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "";

    std::string match_result = dispatcher.match(routes, request_url);
    EXPECT_TRUE(match_result == NO_MATCH);
}

TEST_F(RequestHandlerDispatcherTest, TestMatchEmptySlashOnlyPath) {
    bool success = parser.Parse("check_mapping_config", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "/";

    std::string match_result = dispatcher.match(routes, request_url);
    EXPECT_TRUE(match_result == NO_MATCH);
}

TEST_F(RequestHandlerDispatcherTest, TestDispatchEcho)
{
    bool success = parser.Parse("check_mapping_config", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "/echo";

    RequestHandler* dispatched_handler = dispatcher.dispatch(request_url);
    EXPECT_TRUE(typeid(*dispatched_handler) == typeid(EchoHandler));
}

TEST_F(RequestHandlerDispatcherTest, TestDispatchStatic)
{
    bool success = parser.Parse("check_mapping_config", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "/static/static.txt";

    RequestHandler *dispatched_handler = dispatcher.dispatch(request_url);
    EXPECT_TRUE(typeid(*dispatched_handler) == typeid(StaticHandler));
}

TEST_F(RequestHandlerDispatcherTest, TestDispatch404)
{
    bool success = parser.Parse("check_mapping_config", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "/";

    RequestHandler *dispatched_handler = dispatcher.dispatch(request_url);
    EXPECT_TRUE(typeid(*dispatched_handler) == typeid(ErrorHandler));
}

TEST_F(RequestHandlerDispatcherTest, TestDispatch404PathNotExists)
{
    bool success = parser.Parse("check_mapping_config", &out_config);
    RequestHandlerDispatcher dispatcher(out_config);
    static std::map<std::string, RequestHandlerFactory*> routes = dispatcher.get_routes();
    std::string request_url = "/asdf";

    RequestHandler *dispatched_handler = dispatcher.dispatch(request_url);
    EXPECT_TRUE(typeid(*dispatched_handler) == typeid(ErrorHandler));
}