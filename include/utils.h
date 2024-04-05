#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <boost/beast.hpp>
#include "request_handler_factory.h"

const std::string kStaticHandler = "StaticHandler";
const std::string kEchoHandler = "EchoHandler";
const std::string kErrorHandler = "ErrorHandler";
const std::string kSleepHandler = "SleepHandler";
const std::string kHealthHandler = "HealthHandler";

RequestHandlerFactory *createHandlerFactory(const std::string &name, std::shared_ptr<NginxConfig> config);

bool checkResponseEqual(const http::response<http::string_body> &response1, const http::response<http::string_body> &response2);

#endif // UTILS_H