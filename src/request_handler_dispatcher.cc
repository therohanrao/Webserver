#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>

#include "config_parser.h"
#include "request_handler_dispatcher.h"
#include "request_handler.h"
#include "request_handler_echo.h"
#include "request_handler_static.h"
#include "request_handler_error.h"
#include "request_handler_sleep.h"
#include "request_handler_health.h"

#include "utils.h"

RequestHandlerDispatcher::RequestHandlerDispatcher(NginxConfig config) {
    // Make routes map here with these factories
    config_ = config;
    int error = 0;
    loc_map_ = config_.get_location_mapping(error);
    for(auto it = loc_map_.begin(); it != loc_map_.end(); it++) {
        std::string location_path = it->first;
        std::string handler_type =  it->second.first;
        if(routes_.find(location_path) == routes_.end()) { // location_path is new
            // it.second.second = child block of location statement which contains root parameter
            routes_[location_path] = createHandlerFactory(handler_type, it->second.second);
        }
    }
    routes_[NO_MATCH] = createHandlerFactory(kErrorHandler, nullptr);
}

std::string RequestHandlerDispatcher::get_handler_type()
{
    return handler_type_;
}

RequestHandler* RequestHandlerDispatcher::dispatch(std::string request_url)
{
    std::string location = match(routes_, request_url);

    if(location == NO_MATCH) {
        BOOST_LOG_TRIVIAL(info) << "Created ErrorHandler request handler\n";
        return routes_[NO_MATCH]->create(location, request_url);     
    }

    BOOST_LOG_TRIVIAL(info) << "Created" << loc_map_[location].first << "request handler\n";
    handler_type_ = loc_map_[location].first;
    return routes_[location]->create(location, request_url);       
}

std::string RequestHandlerDispatcher::match(std::map<std::string, RequestHandlerFactory*> routes, std::string request_url)
{
    // Find longest matching prefix
    // Strip off last path until there is a match
    // Return "" if no match
    int url_length = request_url.length();
    std::string curr_url;

    while (url_length > 0)
    {
        curr_url = request_url.substr(0, url_length);
        if (routes.find(curr_url) == routes.end())
        {
            // Update end to index before last slash while no matching prefix found
            url_length = curr_url.find_last_of("/", url_length);
        }
        else
        {
            // Found matching prefix
            return curr_url;
        }
    }

    BOOST_LOG_TRIVIAL(error) << "No matching URL prefix found\n";
    // No match found, return / so 404 handler can handle request as specified in assignment 6 spec
    return NO_MATCH;
}

std::map<std::string, RequestHandlerFactory*> RequestHandlerDispatcher::get_routes() {
    return routes_;
}

RequestHandlerDispatcher::~RequestHandlerDispatcher()
{
    //delete factories
    for(auto it = routes_.begin(); it!=routes_.end(); it++ ) {
        delete it->second;
    }

}