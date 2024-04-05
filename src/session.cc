// source file for session class
// defines following methods for session: initializer, socket(), start(), handle_read(), handle_write()

#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/bind.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "config_parser.h"
#include "request_handler.h"
#include "request_handler_dispatcher.h"
#include "request_handler_echo.h"
#include "request_handler_static.h"
#include "session.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

session::session(boost::asio::io_service &io_service, NginxConfig config_, RequestHandlerDispatcher *dispatcher)
    : socket_(io_service), config_(config_), dispatcher_(dispatcher)
{
}

tcp::socket &session::socket()
{
    return socket_;
}

void session::start()
{
    try
    {
        session_ip_address = socket_.remote_endpoint().address().to_string();
    }
    catch (std::exception &e)
    {
        session_ip_address = "Unknown IP address";
    }
    BOOST_LOG_TRIVIAL(info) << "Started new session with IP address " << session_ip_address;

    // At the start of session the request is empty an end of request is not reached yet
    request = "";

    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&session::handle_read, this, boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code &error, size_t bytes_transferred)
{
    if (!error)
    {
        BOOST_LOG_TRIVIAL(info) << "Reading HTTP request from " << session_ip_address;
        handle_request(bytes_transferred);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Error reading request";
        delete this;
    }
}

void session::handle_request(size_t bytes_transferred)
{
    std::string data_copy = std::string(data_);
    std::string body_part = data_copy;

    request = request + data_copy;
    if (!headers_done_)
    {
        // Returns first index of matching substring if found, or -1 (std::string::npos) if not
        int find_CRLF = request.find("\r\n\r\n");

        // If found CRLF, then the headers are done; read the headers
        if (find_CRLF != std::string::npos)
        {
            headers_done_ = true;

            // The +4 is for the double CRLF.
            std::string headers = request.substr(0, find_CRLF + 4);

            boost::system::error_code ec;
            parser_.put(boost::asio::const_buffer(headers.data(), headers.size()), ec);
            if (ec && ec.value() != 3)
            {
                // Unable to parse HTTP request, meaning malformed request, return 404 response
                BOOST_LOG_TRIVIAL(error) << "Request could not be parsed (invalid): returning 404 response\n" << request << "\n";

                res.version(11);
                res.result(http::status::bad_request);
                res.set(http::field::content_type, "text/plain");
                res.body() = "Malformed HTTP request, unable to parse\r\n";
                res.prepare_payload();

                BOOST_LOG_TRIVIAL(info) << "Writing response to " << session_ip_address << "\n";
                http::async_write(socket_, res, boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
                // Logging for machine-parseable output
                int responseCode = res.result_int();
                std::string requestPath = "/";
                std::string handler_type = "ErrorHandler";
                BOOST_LOG_TRIVIAL(info) << "[ResponseMetrics] "
                                        << "Response code: " << responseCode << " Request path: " << requestPath << " Request IP: " << session_ip_address << " Request Handler: " << handler_type << "\n";
                return;
            }

            // If headers are done, check for content-length
            auto re = parser_.get();
            auto cl_it = re.find(http::field::content_length);
            if (cl_it != re.end())
            {
                // There is a content-length. Save it and record it.
                std::string cl_str = std::string(cl_it->value().data(), cl_it->value().size());
                content_left_ = std::stoi(cl_str);

                // Set body_part to after the headers, instead of all of data.
                body_part = request.substr(find_CRLF + 4);
            }
            else
            {
                body_done_ = true;
            }
        }
    }

    if (!body_done_)
    {
        body_ += body_part;
        content_left_ -= body_part.size();
        if (content_left_ <= 0)
        {
            // Done; just put what you have in the request body and the parser
            body_done_ = true;
        }
        // else, keep reading data
    }

    if (headers_done_ && body_done_)
    {
        req = parser_.release();
        req.body() = body_;

        auto req_uri = req.target();

        RequestHandler *handler = dispatcher_->dispatch(std::string(req_uri.data(), req_uri.size()));
        handler->handle_request(req, res);
        delete handler;

        BOOST_LOG_TRIVIAL(info) << "Writing response to " << session_ip_address << "\n";
        http::async_write(socket_, res, boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
        // Logging for machine-parseable output
        int responseCode = res.result_int();
        std::string requestPath = std::string(req_uri.data(), req_uri.size());
        std::string handler_type = dispatcher_->get_handler_type();
        BOOST_LOG_TRIVIAL(info) << "[ResponseMetrics] " << "Response code: " << responseCode << " Request path: " << requestPath << " Request IP: " << session_ip_address << " Request Handler: " << handler_type << "\n";
    }
    // Otherwise, keep reading input until headers/body is done
    else
    {
        BOOST_LOG_TRIVIAL(trace) << "Continuing reading request from " << session_ip_address;
        memset(data_, '\0', max_length);
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                boost::bind(&session::handle_read, this, boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
}

void session::handle_write(const boost::system::error_code &error)
{
    if (!error)
    {
        BOOST_LOG_TRIVIAL(info) << "Completed writing response to " << session_ip_address;
        // close connection to socket since we're done reading
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Error writing response to " << session_ip_address;
        delete this;
    }
}