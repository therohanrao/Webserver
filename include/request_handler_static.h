/**
 * Request handler for serving static files.
 * 
 * StaticHandler have to be explicitly initialized with a file root path.
 */

#ifndef REQUEST_HANDLER_STATIC_H
#define REQUEST_HANDLER_STATIC_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <map>
#include <boost/beast/http.hpp>

#include "config_parser.h"
#include "request_handler.h"

class StaticHandler : public RequestHandler {
public:
    StaticHandler(const std::string location, const std::string request_url, const std::string root);
    virtual status handle_request(const request req, response& res);

private:
    std::string location_;
    std::string request_url_;
    std::string root_;
};

#endif  // REQUEST_HANDLER_STATIC_H