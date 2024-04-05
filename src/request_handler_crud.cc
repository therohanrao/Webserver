#include "request_handler_crud.h"
#include "filesystem.h"
#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <regex>
#include <string>
#include <vector>

namespace http = boost::beast::http;

CrudHandler::CrudHandler(const std::string location, const std::string request_url, const std::string data_path,
                         std::unique_ptr<FileSystem> fs)
    : location_(location), request_url_(request_url), data_path_(data_path), fs_(std::move(fs))
{
}

/*
 * Handles CRUD requests.
 *
 * Calls the appropriate handle_<action>() private method, which may throw an HttpException if it encounters errors.
 * If so, handle_request() will catch the exception and modify the HTTP response to describe the caught HTTP error.
 *
 * May return HTTP error response...
 *   - 400 (Bad Request)           if thrown by handle_<action>()
 *   - 404 (Not Found)             if thrown by handle_<action>()
 *   - 500 (Internal Server Error) if thrown by handle_<action>()
 *   - 501 (Not Implemented)       if request method is unsupported (i.e. neither GET, POST, PUT, nor DELETE)
 */
status CrudHandler::handle_request(const request req, response &res)
{
    // Target is equal to portion of the request URL after the longest matching prefix
    std::string target = request_url_.substr(location_.length());

    try
    {
        switch (req.method())
        {
        case http::verb::get:
            res.version(req.version());
            handle_get(target, res);
            return true;
        case http::verb::post:
            res.version(req.version());
            handle_post(target, req.body().data(), res);
            return true;
        case http::verb::put:
            res.version(req.version());
            handle_put(target, req.body().data(), res);
            return true;
        case http::verb::delete_:
            res.version(req.version());
            handle_delete(target, res);
            return true;
        default:
            throw HttpException(http::status::not_implemented, "Unsupported HTTP method");
        }
    }
    catch (HttpException &e)
    {
        // Return HTTP error response
        res.version(req.version());
        res.result(e.result());
        res.set(http::field::content_type, "text/plain");
        res.body() = e.what();
        res.prepare_payload();
        return false;
    }
}

/*
 * Handles GET requests (Retrieve and List CRUD actions).
 *
 * Throws HttpException...
 *   - 404 (Not Found) if the requested resource doesn't exist
 *   - 400 (Bad Request) if the requested resource to be retrieved has invalid ID or if the requested resource
 *                       corresponds to neither a regular file (entity) nor a directory (entity type)
 *   - 500 (Internal Server Error) if an internal filesystem error occured
 */
status CrudHandler::handle_get(std::string target, response &res)
{
    std::string local_file_path = data_path_ + target;
    if (!fs_->exists(local_file_path))
    {
        throw HttpException(http::status::not_found, "Requested resource " + target + " does not exist");
    }

    if (fs_->is_regular_file(local_file_path))
    {
        // Retrieve entity
        std::string entity_id = local_file_path.substr(local_file_path.find_last_of("/") + 1);
        if (!is_valid_id(entity_id))
        {
            throw HttpException(http::status::bad_request, "Requested entity has invalid ID " + entity_id);
        }

        std::string entity = fs_->get(local_file_path);
        if (fs_->err())
        {
            throw HttpException(http::status::internal_server_error,
                                "Error occurred when performing Retrieve operation");
        }

        res.body() = entity;
    }
    else if (fs_->is_directory(local_file_path))
    {
        // List entity IDs
        std::vector<std::string> entity_ids = fs_->list_dir(local_file_path);
        if (fs_->err())
        {
            throw HttpException(http::status::internal_server_error, "Error occurred when performing List operation");
        }

        // Filter out non-entity filenames (i.e. filenames which do not correspond to valid entity IDs)
        entity_ids.erase(
            std::remove_if(entity_ids.begin(), entity_ids.end(), [this](std::string id) { return !is_valid_id(id); }),
            entity_ids.end());
        res.body() = "[" + boost::algorithm::join(entity_ids, ", ") + "]";
    }
    else
    {
        throw HttpException(http::status::bad_request,
                            "Requested resource " + target + " is neither an entity nor an entity type");
    }

    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    res.prepare_payload();
    return true;
}

/*
 * Handles POST requests (Create CRUD actions).
 *
 * Throws HttpException...
 *   - 400 (Bad Request) if the requested resource at which to create a new entity is not a directory (entity type)
 *   - 500 (Internal Server Error) if an internal filesystem error occured
 */
status CrudHandler::handle_post(std::string target, std::string body, response &res)
{
    std::string local_file_path = data_path_ + target;
    if (fs_->exists(local_file_path) && !fs_->is_directory(local_file_path))
    {
        throw HttpException(http::status::bad_request, "Requested resource " + target + " is not an entity type");
    }

    std::string new_entity_id = get_next_available_id(local_file_path);
    std::string new_entity_filepath = local_file_path + "/" + new_entity_id;

    fs_->write(new_entity_filepath, body);
    if (fs_->err())
    {
        throw HttpException(http::status::internal_server_error, "Error occurred when performing Create operation");
    }

    res.result(http::status::created);
    res.set(http::field::content_type, "application/json");
    res.body() = "{\"id\": " + new_entity_id + "}";
    res.prepare_payload();
    return true;
}

/*
 * Handles PUT requests (Update CRUD actions).
 *
 * Throws HttpException...
 *   - 400 (Bad Request) if the requested resource to update has invalid entity ID or is not a regular file (entity)
 *   - 500 (Internal Server Error) if an internal filesystem error occured
 */
status CrudHandler::handle_put(std::string target, std::string body, response &res)
{
    std::string local_file_path = data_path_ + target;

    std::string entity_id = local_file_path.substr(local_file_path.find_last_of("/") + 1);
    if (!is_valid_id(entity_id))
    {
        throw HttpException(http::status::bad_request, "Requested entity has invalid ID " + entity_id);
    }

    if (fs_->exists(local_file_path) && !fs_->is_regular_file(local_file_path))
    {
        throw HttpException(http::status::bad_request, "Requested resource " + target + " is not an entity");
    }

    http::status success_status = fs_->exists(local_file_path) ? http::status::ok : http::status::created;

    fs_->write(local_file_path, body);
    if (fs_->err())
    {
        throw HttpException(http::status::internal_server_error, "Error occurred when performing Update operation");
    }

    res.result(success_status);
    res.set(http::field::content_type, "text/plain");
    res.body() = "";
    res.prepare_payload();
    return true;
}

/*
 * Handles DELETE requests (Delete CRUD actions).
 *
 * Throws HttpException...
 *   - 400 (Bad Request) if the requested resource to delete has invalid entity ID or is not a regular file (entity)
 *   - 404 (Not Found) if the requested resource to delete doesn't exist
 *   - 500 (Internal Server Error) if an internal filesystem error occured
 */
status CrudHandler::handle_delete(std::string target, response &res)
{
    std::string local_file_path = data_path_ + target;

    std::string entity_id = local_file_path.substr(local_file_path.find_last_of("/") + 1);
    if (!is_valid_id(entity_id))
    {
        throw HttpException(http::status::bad_request, "Requested entity has invalid ID " + entity_id);
    }

    if (!fs_->exists(local_file_path))
    {
        throw HttpException(http::status::not_found, "Requested resource " + target + " does not exist");
    }
    if (!fs_->is_regular_file(local_file_path))
    {
        throw HttpException(http::status::bad_request, "Requested resource " + target + " is not an entity");
    }

    fs_->remove(local_file_path);
    if (fs_->err())
    {
        throw HttpException(http::status::internal_server_error, "Error occurred when performing Delete operation");
    }

    res.result(http::status::ok);
    res.set(http::field::content_type, "text/plain");
    res.body() = "";
    res.prepare_payload();
    return true;
}

// Checks whether or not a given string corresponds to a valid entity ID (i.e. consists of only digits).
bool CrudHandler::is_valid_id(std::string name)
{
    std::smatch match;
    return std::regex_match(name, match, std::regex("^[0-9]+$"));
}

// Gets the next available entity ID for a given entity type.
std::string CrudHandler::get_next_available_id(std::string entity_type)
{
    if (!fs_->exists(entity_type))
    {
        return "0";
    }

    std::vector<std::string> filenames = fs_->list_dir(entity_type);
    if (fs_->err())
    {
        throw HttpException(http::status::internal_server_error,
                            "Error occurred when getting next available entity ID");
    }

    // Filter out non-entity filenames (i.e. filenames which do not correspond to valid entity IDs)
    filenames.erase(
        std::remove_if(filenames.begin(), filenames.end(), [this](std::string id) { return !is_valid_id(id); }),
        filenames.end());

    // Convert entity IDs into integers and sort them in ascending order
    std::vector<int> entity_ids;
    std::transform(filenames.begin(), filenames.end(), std::back_inserter(entity_ids),
                   [](std::string id_str) { return std::stoi(id_str); });
    sort(entity_ids.begin(), entity_ids.end());

    // Return the first unused entity ID (in ascending order)
    int new_id = 0;
    while (new_id < entity_ids.size() && new_id == entity_ids[new_id])
    {
        ++new_id;
    }
    return std::to_string(new_id);
}
