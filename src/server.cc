// source file for server class
// defines following methods for server class: initializer, start_accept(), handle_accept() 

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdlib>
#include <iostream>

#include "server.h"
#include "session.h"
#include "request_handler_dispatcher.h"

server::server(boost::asio::io_service &io_service, short port, int num_threads, NginxConfig config_)
    : io_service_(io_service),
    acceptor_(io_service_, tcp::endpoint(tcp::v4(), port)), config_(config_)
{
    BOOST_LOG_TRIVIAL(info) << "Started server at port " << port;
    dispatcher_ = new RequestHandlerDispatcher(config_);
    thread_pool_size_ = num_threads;
    start_accept();
}

void server::run()
{
    // Create a pool of threads to run all of the io_services.
    std::vector<boost::shared_ptr<boost::thread>> threads;    
    for (std::size_t i = 0; i < thread_pool_size_; ++i)
    {
      boost::shared_ptr<boost::thread> thread(new boost::thread(
        boost::bind(&boost::asio::io_service::run, &io_service_)));
      threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < threads.size(); ++i)
      threads[i]->join();
}

void server::start_accept()
{
    session* new_session = new session(io_service_, config_, dispatcher_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
      const boost::system::error_code& error)
{
    if (!error)
    {
      new_session->start();
    }
    else
    {
      BOOST_LOG_TRIVIAL(error) << "Session failed to start";
      delete new_session;
      new_session = nullptr; // fixes dangling pointer
    }

    start_accept();
}

server::~server() {
    delete dispatcher_;
}