/**
 * Request handler for echo.
 */

#ifndef REQUEST_HANDLER_ECHO_H
#define REQUEST_HANDLER_ECHO_H

#include <iostream>
#include <boost/beast/http.hpp>

#include "request_handler.h"

class EchoHandler : public RequestHandler {
public:
    EchoHandler(const std::string location, const std::string request_url);
    virtual status handle_request(const request req, response& res);
private:
    std::string location_;
    std::string request_url_;
};

#endif  // REQUEST_HANDLER_ECHO_H