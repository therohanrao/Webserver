// source file for server initializer class
// defines following methods for server class: get_server(), handle_accept() 

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>

#include "server_init.h"
#include "server.h"

using namespace std; // For atoi.

server_init::server_init(boost::asio::io_service &io_service, std::istream *config_file)
{
    // Take command line argument as file name for server config file
    BOOST_LOG_TRIVIAL(info) << "Parsing config file started";
    config_parser_.Parse(config_file, &config_);
    BOOST_LOG_TRIVIAL(info) << "Parsing config file completed";
    // Start the server at the port number from the config file
    int port = config_.get_port();
    int num_threads = config_.get_max_threads();
    s_ = new server(io_service, (short)port, num_threads, config_);

    s_->run();
}

server_init::server_init(boost::asio::io_service &io_service, const char *file_name)
{
    // Take command line argument as file name for server config file
    BOOST_LOG_TRIVIAL(info) << "Parsing config file started";
    config_parser_.Parse(file_name, &config_);
    BOOST_LOG_TRIVIAL(info) << "Parsing config file completed";
    // Start the server at the port number from the config file
    int port = config_.get_port();
    BOOST_LOG_TRIVIAL(info) << "Starting server at port " << port;

    // Initialise the server.
    int num_threads = config_.get_max_threads();
    if (num_threads == -1) {
        BOOST_LOG_TRIVIAL(error) << "Unable to parse configured number of threads; using default of 10";
        num_threads = 10;
    }

    BOOST_LOG_TRIVIAL(info) << "Starting server with " << num_threads << " threads";
    s_ = new server(io_service, (short)port, num_threads, config_);

    // Run the server until stopped.
    s_->run();

}

server_init::~server_init()
{
    delete s_;
}

server* server_init::get_server()
{
    return s_;
}

void server_init::handle_accept(session *new_session, const boost::system::error_code& error)
{
 s_->handle_accept(new_session, error);
}