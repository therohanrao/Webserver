#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include "gtest/gtest.h"
#include <fstream>

#include "config_parser.h"
#include "server_init.h"
#include "session.h"

class ServerTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    NginxConfigParser parser;
    NginxConfig config_;
};

TEST_F(ServerTest, ServerStarts) {
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


TEST_F(ServerTest, PassDummyAccept) {
 bool success = false;

 boost::system::error_code no_error = boost::system::errc::make_error_code(boost::system::errc::success);

 try
 {
 boost::asio::io_service io_service;
 server_init s_i(io_service, "single_quote");
 parser.Parse("single_quote", &config_);
 RequestHandlerDispatcher dispatcher(config_);
 session sesh(io_service, config_, &dispatcher);
 s_i.handle_accept(&sesh, no_error);
 if(s_i.get_server())
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


TEST_F(ServerTest, FailDummyAccept) {
 bool success = false;

 boost::system::error_code not_supported_err = boost::system::errc::make_error_code(boost::system::errc::not_supported);

 try
 {
 boost::asio::io_service io_service;
 server_init s_i(io_service, "single_quote");
 parser.Parse("single_quote", &config_);
 RequestHandlerDispatcher dispatcher(config_);
 session* sesh = new session(io_service, config_, &dispatcher);
 //io_service.run();
 s_i.handle_accept(sesh, not_supported_err);
 success = true;
 }
 catch (std::exception& e)
 {
    success = false;
 }

 EXPECT_TRUE(success);
}
