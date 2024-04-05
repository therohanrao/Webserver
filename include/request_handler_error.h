/**
 * Request handler for 404 response.
 */

#ifndef REQUEST_HANDLER_ERROR_H
#define REQUEST_HANDLER_ERROR_H

#include <iostream>

#include "request_handler.h"
#include <boost/beast/http.hpp>

class ErrorHandler : public RequestHandler
{
public:
    ErrorHandler(const std::string location, const std::string request_url);
    virtual status handle_request(const request req, response& res);
private:
    std::string location_;
    std::string request_url_;
};

#endif // REQUEST_HANDLER_ERROR_H