#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/bind.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <cassert>
#include <iostream>

#include "request_handler_echo.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

// Handler passed in location and request_url to conform with common API spec, but they are not used
EchoHandler::EchoHandler(const std::string location, const std::string request_url)
{
    location_ = location;
    request_url_ = request_url;
}

/**
 * handle_request() - Return response same as request.
 */
status EchoHandler::handle_request(const request req, response &res)
{
    res.version(req.version());
    res.result(http::status::ok);
    res.reason("OK");
    res.set(http::field::content_type, "text/plain");
    {
        // Set the response body by converting req to a string
        std::ostringstream oss;
        oss << req;
        res.body() = oss.str();
        oss.clear();
    }
    res.prepare_payload();
    return true;
}