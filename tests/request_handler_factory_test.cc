#include <boost/beast.hpp>
#include <iostream>
#include "gtest/gtest.h"
#include <string>
#include <vector>


#include "request_handler_factory.h"
#include "config_parser.h"
#include "utils.h"

class RequestHandlerFactoryTest : public ::testing::Test
{
protected:
    NginxConfigParser config_parser;
    NginxConfig config;
};

// A test to check if the output of a text file request is what it should be (an
// OK and echoing of the text file)
TEST_F(RequestHandlerFactoryTest, CreateAllFactories)
{
    bool success = config_parser.Parse("static_config", &config);

    std::vector<std::string> handler_types{"StaticHandler", "EchoHandler", "ErrorHandler"}; 
    std::vector<RequestHandlerFactory*> factories;

    for(int i = 0; i < handler_types.size(); i++) {
        factories.push_back(nullptr);
    }

    for(int i = 0; i < handler_types.size(); i++) {
        factories[i] = createHandlerFactory(handler_types[i], nullptr);
    }

    bool all_factories_created = true;

    for(int i = 0; i < handler_types.size(); i++) {
        if(factories[i] == nullptr) {
            all_factories_created = false;
        }
    }

    EXPECT_TRUE(all_factories_created);
}
