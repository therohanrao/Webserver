#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <cassert>
#include <iostream>

#include "request_handler_health.h"

using boost::asio::ip::tcp;

// Handler passed in location and request_url to conform with common API spec, but they are not used
HealthHandler::HealthHandler(const std::string location, const std::string request_url)
{
    location_ = location;
    request_url_ = request_url;
}

/**
 * handle_request() - return the health of the server (hopefully just an OK 200 response)
 */
status HealthHandler::handle_request(const request req, response& res)
{
    
    std::string ok_body = "OK\r\n";
    res.version(req.version());
    res.result(http::status::ok);
    res.reason("OK");
    res.set(http::field::content_type, "text/plain");
    res.set(http::field::content_length, std::to_string(ok_body.length()));
    res.body() = ok_body;
    res.prepare_payload();
    return true;
}