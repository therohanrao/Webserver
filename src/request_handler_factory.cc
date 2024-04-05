
#include<memory>
#include <string>
#include <stdexcept>

#include "request_handler_factory.h"
#include "request_handler_static.h"
#include "request_handler_echo.h"
#include "request_handler_crud.h"
#include "request_handler_error.h"
#include "request_handler_sleep.h"
#include "request_handler_health.h"
#include "config_parser.h"
#include "filesystem_disk.h"

StaticHandlerFactory::StaticHandlerFactory(std::shared_ptr<NginxConfig> config)
{
    config_ = config;
}

CrudHandlerFactory::CrudHandlerFactory(std::shared_ptr<NginxConfig> config)
{
    config_ = config;

}

RequestHandler* CrudHandlerFactory::create(const std::string& location, const std::string& url) {
    std::string root = config_->get_data_path();
    return new CrudHandler(location, url, root, std::make_unique<DiskFileSystem>());
}

RequestHandler* StaticHandlerFactory::create(const std::string& location, const std::string& url) {
    std::string root = config_->get_root();
    return new StaticHandler(location, url, root);
}

RequestHandler* EchoHandlerFactory::create(const std::string& location, const std::string& url) {
    return new EchoHandler(location, url);
}

RequestHandler* ErrorHandlerFactory::create(const std::string& location, const std::string& url) {
    return new ErrorHandler(location, url);
}

RequestHandler* SleepHandlerFactory::create(const std::string& location, const std::string& url) {
     return new SleepHandler(location, url);
}

RequestHandler* HealthHandlerFactory::create(const std::string& location, const std::string& url) {
    return new HealthHandler(location, url);
}
