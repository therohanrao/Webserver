/**
 * Abstract class for request handlers.
 */

#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <iostream>
#include <boost/beast/http.hpp>

#include "config_parser.h"

namespace http = boost::beast::http;

typedef http::request<http::string_body> request;
typedef http::response<http::string_body> response;
typedef bool status;

class RequestHandler {
public:
    virtual status handle_request(const request req, response& res) = 0;
};

#endif  // REQUEST_HANDLER_H