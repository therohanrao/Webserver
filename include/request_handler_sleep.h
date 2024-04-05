#ifndef REQUEST_HANDLER_SLEEP_H
#define REQUEST_HANDLER_SLEEP_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <map>
#include <boost/beast/http.hpp>

#include "request_handler.h"

class SleepHandler : public RequestHandler {
public:
    SleepHandler(const std::string location, const std::string request_url);
    virtual status handle_request(const request req, response& res);

private:
    std::string location_;
    std::string request_url_;
};

#endif  // REQUEST_HANDLER_SLEEP_H