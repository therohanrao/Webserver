// header file for session class

#ifndef SESSION_H
#define SESSION_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <boost/beast/http.hpp>

#include "request_handler_echo.h"
#include "request_handler_static.h"
#include "request_handler_dispatcher.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

class session
{
public:
 friend class session_wrapper;
  session(boost::asio::io_service& io_servic, NginxConfig config_, RequestHandlerDispatcher* dispatcher);
  tcp::socket& socket();
  void start();

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  void handle_write(const boost::system::error_code& error);

  void handle_request(size_t bytes_transferred);

  std::string parse_start_line(std::string line);

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  // Store request to be echoed back in response
  std::string request;
  std::string body_;
  bool headers_done_ = false;
  bool body_done_ = false;
  int content_left_ = 0;
  http::request<http::string_body> req;
  std::string session_ip_address;
  http::response<http::string_body> res;
  NginxConfig config_;
  RequestHandlerDispatcher* dispatcher_;
  http::request_parser<http::string_body> parser_;
};
#endif // SESSION_H