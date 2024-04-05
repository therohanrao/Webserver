#include <iostream>
#include <boost/beast/http.hpp>

#include "request_handler_static.h"
#include "config_parser.h"

namespace http = boost::beast::http;

// Adapted from: https://www.boost.org/doc/libs/1_78_0/doc/html/boost_asio/examples/cpp11_examples.html
std::string extension_to_type(const std::string &extension)
{

    struct mapping
    {
        const char *extension;
        const char *mime_type;
    } mappings[] =
        {
            {"gif", "image/gif"},
            {"htm", "text/html"},
            {"html", "text/html"},
            {"jpg", "image/jpeg"},
            {"png", "image/png"},
            {"pdf", "application/pdf"},
            {"zip", "application/zip"}};

    for (mapping m : mappings)
    {
        if (m.extension == extension)
        {
            return m.mime_type;
        }
    }

    return "text/plain";
}

StaticHandler::StaticHandler(const std::string location, const std::string request_url, const std::string root) {
    this->location_ = location;
    this->request_url_ = request_url;
    this->root_ = root;
}

/**`
 * handle_request() - Fill response with static files.
 */
status StaticHandler::handle_request(const request req, response& res)
{
    std::string extension;

    size_t cursor = request_url_.find_last_of(".");
    if (cursor != std::string::npos) {
        extension = request_url_.substr(cursor + 1);
    } else {
        return false;
    }

    std::string content_type = extension_to_type(extension);

    // Requested file is equal to remaining portion of URL after longest matching prefix
    std::string requested_file_path = request_url_.substr(location_.length(), std::string::npos);
    // Server binary is on the same level as static folders in production
    // So relative path of ./ can be accessed without any modification
    // Absolute path can also be appended without modification as it is whole path definition
    std::string local_file_path = root_ + requested_file_path;

    std::ifstream f;
    f = std::ifstream(local_file_path, std::ios::in | std::ios::binary);

    if (!f)
    {
        std::string file_not_found = "File not found.\r\n";
        res.version(req.version());
        res.result(http::status::not_found);
        res.set(http::field::content_type, "text/plain");
        res.set(http::field::content_length, std::to_string(file_not_found.length()));
        res.body() = file_not_found;
        return false;
    }

    // Read file
    std::string body;
    char c;
    while (f.get(c)) {
        body += c;
    }
    f.close();

    res.version(req.version());
    res.result(http::status::ok);
    res.reason("OK");
    if (req.has_content_length())
    {
        res.set(http::field::content_length, req[http::field::content_length]);
    }
    res.set(http::field::content_type, content_type);
    res.body() = body;
    res.prepare_payload();
    return true;
}