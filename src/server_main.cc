//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <cstdlib>
#include <iostream>
#include <signal.h>

#include "config_parser.h"
#include "server.h"
#include "session.h"
#include "server_init.h"

using boost::asio::ip::tcp;

static void init_log()
{
  static const std::string COMMON_FMT("[%TimeStamp%][%ThreadID%][%Severity%]:  %Message%");

  boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

  // Output message to console
  boost::log::add_console_log(
      std::cout,
      boost::log::keywords::format = COMMON_FMT,
      boost::log::keywords::auto_flush = true);

  // Output message to file, rotates when file reached 1mb or at midnight every day. Each log file
  // is capped at 1mb and total is 20mb
  boost::log::add_file_log(
      boost::log::keywords::file_name = "../src/syslog_%N.log",
      boost::log::keywords::rotation_size = 1 * 1024 * 1024,
      boost::log::keywords::max_size = 20 * 1024 * 1024,
      boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
      boost::log::keywords::format = COMMON_FMT,
      boost::log::keywords::auto_flush = true);

  boost::log::add_common_attributes();

  // Only output message with INFO or higher severity in Release
  #ifndef _DEBUG
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::info);
  #endif
}

void signalHandler(int signalNum) {
    BOOST_LOG_TRIVIAL(fatal) << "Server terminated with signal " << signalNum;

    exit(signalNum);
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <config_file>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    using namespace std; // For atoi.

    init_log();

    signal(SIGINT, signalHandler);

    server_init server_initializer(io_service, argv[1]);
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
