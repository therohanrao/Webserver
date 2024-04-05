#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <cassert>
#include <iostream>

#include "request_handler_sleep.h"

using boost::asio::ip::tcp;

// Handler passed in location and request_url to conform with common API spec, but they are not used
SleepHandler::SleepHandler(const std::string location, const std::string request_url)
{
    location_ = location;
    request_url_ = request_url;
}

/**
 * handle_request() - sleep
 */
status SleepHandler::handle_request(const request req, response& res)
{
    int sleep_duration_s = 5;
    std::string slept = "Slept for " + std::to_string(sleep_duration_s) + " seconds.\r\n";
    res.version(req.version());
    res.result(http::status::ok);
    res.reason("OK");
    res.set(http::field::content_type, "text/plain");
    res.set(http::field::content_length, std::to_string(slept.length()));
    res.body() = slept;
    res.prepare_payload();
    sleep(sleep_duration_s);
    return true;
}