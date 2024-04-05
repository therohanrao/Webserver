#ifndef REQUEST_HANDLER_DISPATCHER_H
#define REQUEST_HANDLER_DISPATCHER_H

#include <iostream>
#include <map>

#include "config_parser.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class RequestHandlerDispatcher
{
public:
    // To conform with sibling request_handler_static class
    RequestHandlerDispatcher(NginxConfig config);
    std::string get_handler_type();
    RequestHandler* dispatch(std::string request_url);
    std::string match(std::map<std::string, RequestHandlerFactory*> routes, std::string request_url);
    std::map<std::string, RequestHandlerFactory*> get_routes();
    ~RequestHandlerDispatcher();

private:
    const std::string NO_MATCH = "/";
    NginxConfig config_;
    std::string handler_type_;
    std::map<std::string, RequestHandlerFactory*> routes_;
    std::map<std::string, std::pair<std::string, std::shared_ptr<NginxConfig>>> loc_map_;
};

#endif // REQUEST_HANDLER_DISPATCHER_H