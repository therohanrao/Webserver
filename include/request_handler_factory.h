
#ifndef REQUEST_HANDLER_FACTORY_H
#define REQUEST_HANDLER_FACTORY_H

#include <iostream>
#include <map>

#include "request_handler.h"
#include "config_parser.h"

class RequestHandlerFactory {
public:
    // To conform with sibling request_handler_static class
    virtual RequestHandler* create(const std::string& location, const std::string& url) = 0;
};

class CrudHandlerFactory : public RequestHandlerFactory {
public:
    CrudHandlerFactory(std::shared_ptr<NginxConfig> config);
    virtual RequestHandler* create(const std::string& location, const std::string& url);
private:
    std::shared_ptr<NginxConfig> config_;
};

class StaticHandlerFactory : public RequestHandlerFactory {
public:
    StaticHandlerFactory(std::shared_ptr<NginxConfig> config);
    virtual RequestHandler* create(const std::string& location, const std::string& url);
private:
    std::shared_ptr<NginxConfig> config_;
};

class EchoHandlerFactory : public RequestHandlerFactory {
public:
    virtual RequestHandler* create(const std::string& location, const std::string& url);
};

class ErrorHandlerFactory : public RequestHandlerFactory {
public:
    virtual RequestHandler* create(const std::string& location, const std::string& url);
};

class SleepHandlerFactory : public RequestHandlerFactory {
public:
    virtual RequestHandler* create(const std::string& location, const std::string& url);
};

class HealthHandlerFactory : public RequestHandlerFactory {
public:
    virtual RequestHandler* create(const std::string& location, const std::string& url);
};

#endif // REQUEST_HANDLER_FACTORY_H


