// header file for server initializer class

#ifndef SERVER_INIT_H
#define SERVER_INIT_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>

#include "config_parser.h"
#include "server.h"
#include "session.h"


class server_init
{
public:
  server_init(boost::asio::io_service &io_service, std::istream *config_file);
  server_init(boost::asio::io_service &io_service, const char *file_name);
  ~server_init();
  
  server* get_server();
  void handle_accept(session *new_session, const boost::system::error_code& error);
private:
  NginxConfigParser config_parser_;
  NginxConfig config_;
  server* s_;
};

#endif // SERVER_INIT_H