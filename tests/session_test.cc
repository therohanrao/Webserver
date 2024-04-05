#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "gtest/gtest.h"
#include <boost/system/error_code.hpp>
#include <fstream>

#include "config_parser.h"
#include "server_init.h"
#include "session.h"

class session_wrapper {
    
  public:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred) 
    {
      sesh->handle_read(error, bytes_transferred);
    }

   void handle_write(const boost::system::error_code& error)
   {
      sesh->handle_write(error);
   }

   void handle_request(size_t bytes_transferred)
   {
      sesh->handle_request(bytes_transferred);
   }

   void set_data(std::string data) //max_length = 1024
   {
    for(int i = 0; i < data.length() && i < 1024; i++)
    {
      sesh->data_[i] = data[i];
    }
   }

   void set_session_ip_address() {
    sesh->session_ip_address = "0.0.0.0";
   }

   void set_dispatcher(RequestHandlerDispatcher* dis)
   {
    sesh->dispatcher_ = dis;
   }

   std::string get_request()
   {
    return sesh->request;
   }

   std::string get_request_body()
   {
    return sesh->body_;
   }

   http::response<http::string_body> get_res()
   {
    return sesh->res;
   }

  private:
    boost::asio::io_service io_service;
    NginxConfig config_;
    RequestHandlerDispatcher* dispatcher_;
    session* sesh = new session(io_service, config_, dispatcher_);
};


class SessionTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    session_wrapper sesh_w;
    size_t NUM_BYTES_TO_HANDLE = 100;
};

TEST_F(SessionTest, HandleReadSuccess) {
 bool success = false;
 NUM_BYTES_TO_HANDLE = 100;
 boost::system::error_code no_error = boost::system::errc::make_error_code(boost::system::errc::success);

 sesh_w.handle_read(no_error, NUM_BYTES_TO_HANDLE);
 success = true;

 EXPECT_TRUE(success);
}

TEST_F(SessionTest, HandleReadNotSupported) {
 bool success = false;

 boost::system::error_code not_supported_err = boost::system::errc::make_error_code(boost::system::errc::not_supported);

 session_wrapper sesh_w;
 sesh_w.handle_read(not_supported_err, NUM_BYTES_TO_HANDLE);
 success = true;

 EXPECT_TRUE(success);
}

TEST_F(SessionTest, HandleWriteSuccess) {
 bool success = false;

 boost::system::error_code no_error = boost::system::errc::make_error_code(boost::system::errc::success);

 sesh_w.handle_write(no_error);
 success = true;

 EXPECT_TRUE(success);
}

TEST_F(SessionTest, HandleWriteNotSupported) {
 bool success = false;

 boost::system::error_code not_supported_err = boost::system::errc::make_error_code(boost::system::errc::not_supported);

 session_wrapper sesh_w;
 sesh_w.handle_write(not_supported_err);
 success = true;

 EXPECT_TRUE(success);
}

TEST_F(SessionTest, HandleReadData)
{
 bool success = false;
 NUM_BYTES_TO_HANDLE = 100;
 boost::system::error_code no_error = boost::system::errc::make_error_code(boost::system::errc::success);
 sesh_w.set_session_ip_address();

 NginxConfigParser parser;
 NginxConfig out_config;
 parser.Parse("check_mapping_config", &out_config);
 RequestHandlerDispatcher* disp = new RequestHandlerDispatcher(out_config);
 sesh_w.set_dispatcher(disp);

 sesh_w.set_data("GET /echo HTTP/1.1\r\n\r\n");
 sesh_w.handle_read(no_error, NUM_BYTES_TO_HANDLE);
 success = true;

 delete disp;

 EXPECT_TRUE(sesh_w.get_request() == "GET /echo HTTP/1.1\r\n\r\n");
}

TEST_F(SessionTest, HandleReadDataWithBody)
{
 bool success = false;
 NUM_BYTES_TO_HANDLE = 100;
 boost::system::error_code no_error = boost::system::errc::make_error_code(boost::system::errc::success);
 sesh_w.set_session_ip_address();

 NginxConfigParser parser;
 NginxConfig out_config;
 parser.Parse("check_mapping_config", &out_config);
 RequestHandlerDispatcher *disp = new RequestHandlerDispatcher(out_config);
 sesh_w.set_dispatcher(disp);

 sesh_w.set_data("POST /api/Shoes HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: 9\r\n\r\n{\"id\": 0}");
 sesh_w.handle_read(no_error, NUM_BYTES_TO_HANDLE);
 success = true;

 delete disp;

 EXPECT_TRUE(sesh_w.get_request() == "POST /api/Shoes HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: 9\r\n\r\n{\"id\": 0}");
 EXPECT_TRUE(sesh_w.get_request_body() == "{\"id\": 0}");
}

TEST_F(SessionTest, HandleReadMalformedRequest)
{
  bool success = false;
  NUM_BYTES_TO_HANDLE = 100;
  boost::system::error_code no_error = boost::system::errc::make_error_code(boost::system::errc::success);
  sesh_w.set_session_ip_address();

  NginxConfigParser parser;
  NginxConfig out_config;
  parser.Parse("check_mapping_config", &out_config);
  RequestHandlerDispatcher *disp = new RequestHandlerDispatcher(out_config);
  sesh_w.set_dispatcher(disp);

  sesh_w.set_data("GET /echo asdf\r\n\r\n");
  sesh_w.handle_read(no_error, NUM_BYTES_TO_HANDLE);
  success = true;

  http::response<http::string_body> res = sesh_w.get_res();

  delete disp;

  EXPECT_TRUE(sesh_w.get_request() == "GET /echo asdf\r\n\r\n");
  EXPECT_TRUE(res.result_int() == 400);
}