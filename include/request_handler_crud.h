#ifndef DC30F54F_01AC_4095_9088_E50499D81036
#define DC30F54F_01AC_4095_9088_E50499D81036

#include <boost/beast/http.hpp>
#include <memory>
#include <stdexcept>
#include <string>

#include "request_handler.h"

class FileSystem;

class CrudHandler : public RequestHandler
{
  public:
    CrudHandler(const std::string location, const std::string request_url, const std::string data_path,
                std::unique_ptr<FileSystem> fs);
    virtual status handle_request(const request req, response &res);

  private:
    // Note: conversion of string_view to string is needed. Make an explicit copy.
    status handle_get(std::string target, response &res);
    status handle_post(std::string target, std::string body, response &res);
    status handle_put(std::string target, std::string body, response &res);
    status handle_delete(std::string target, response &res);

    // An interface to a filesystem.
    std::unique_ptr<FileSystem> fs_;

    std::string location_;
    std::string request_url_;
    std::string data_path_;

    // Utility functions.
    bool is_valid_id(std::string name);
    std::string get_next_available_id(std::string entity_type);

    // Custom exception for error handling.
    class HttpException : std::exception
    {
      public:
        HttpException(http::status result, std::string message) : result_(result), message_(message)
        {
        }

        http::status result()
        {
            return result_;
        }

        const char *what()
        {
            return message_.c_str();
        }

      private:
        http::status result_;
        std::string message_;
    };
};

#endif /* DC30F54F_01AC_4095_9088_E50499D81036 */
