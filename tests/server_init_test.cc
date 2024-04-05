#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include <fstream>
#include "gtest/gtest.h"

#include "config_parser.h"
#include "server_init.h"
#include "session.h"

class ServerInitTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
};


TEST_F(ServerInitTest, ServerInitFile) {
 bool success = false;
 std::string input_name = "single_quote";
 std::ifstream input_file_f(input_name);
 std::istream& input_file = input_file_f;

 try
 {
 boost::asio::io_service io_service;
 server_init s_i(io_service, &input_file);
 success = true;
 }
 catch (std::exception& e)
 {
    success = false;
 }

 EXPECT_TRUE(success);
}

TEST_F(ServerInitTest, ServerInitString) {
 bool success = false;

 try
 {
 boost::asio::io_service io_service;
 server_init s_i(io_service, "single_quote");
 success = true;
 }
 catch (std::exception& e)
 {
    success = false;
 }

 EXPECT_TRUE(success);
}

TEST_F(ServerInitTest, GetServerFromServerInit) {
 bool success = false;
 std::string input_name = "single_quote";
 std::ifstream input_file_f(input_name);
 std::istream& input_file = input_file_f;

 try
 {
 boost::asio::io_service io_service;
 server_init s_i(io_service, &input_file);
 server* serv = s_i.get_server();
 if(serv)
 {
   success = true;
 }
 }
 catch (std::exception& e)
 {
    success = false;
 }

 EXPECT_TRUE(success);
}