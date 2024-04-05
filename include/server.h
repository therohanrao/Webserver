// header file for server class

#ifndef SERVER_H
#define SERVER_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>

#include "session.h"
#include "request_handler_dispatcher.h"

using boost::asio::ip::tcp;

class server
{
public:
  friend class server_init;
  server(boost::asio::io_service &io_service, short port, int num_threads, NginxConfig config_);
  void run();
  ~server();

private:
  void start_accept();

  void handle_accept(session* new_session,
      const boost::system::error_code& error);

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  NginxConfig config_;
  RequestHandlerDispatcher* dispatcher_;
  std::size_t thread_pool_size_;
};
#endif // SERVER_H