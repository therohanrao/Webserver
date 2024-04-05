#include <string>


#include "request_handler_factory.h"
#include "config_parser.h"
#include "utils.h"

RequestHandlerFactory *createHandlerFactory(const std::string &name, std::shared_ptr<NginxConfig> config)
{
  if (name == kStaticHandler)
    return new StaticHandlerFactory(config);
  if (name == kEchoHandler)
    return new EchoHandlerFactory();
  if (name == kErrorHandler)
    return new ErrorHandlerFactory();
  if (name == "CrudHandler")
    return new CrudHandlerFactory(config);
  if (name == kSleepHandler)
     return new SleepHandlerFactory();
  if (name == kHealthHandler)
    return new HealthHandlerFactory();    
  else
    return nullptr;
}

bool checkResponseEqual(const http::response<http::string_body> &response1, const http::response<http::string_body> &response2)
{
    if (response1.result() != response2.result())
    {
        return false;
    }

    if (response1.version() != response2.version())
    {
        return false;
    }

    if (response1.body() != response2.body())
    {
        return false;
    }

    return true;
}