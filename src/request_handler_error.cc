#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <cassert>
#include <iostream>

#include "request_handler_error.h"

using boost::asio::ip::tcp;

// Handler passed in location and request_url to conform with common API spec, but they are not used
ErrorHandler::ErrorHandler(const std::string location, const std::string request_url)
{
    location_ = location;
    request_url_ = request_url;
}

/**
 * handle_request() - Return 404 error response
 */
status ErrorHandler::handle_request(const request req, response& res)
{
    std::string file_not_found = "File not found.\r\n";
    res.version(req.version());
    res.result(http::status::not_found);
    res.set(http::field::content_type, "text/plain");
    res.set(http::field::content_length, std::to_string(file_not_found.length()));
    res.body() = file_not_found;
    return false;
}