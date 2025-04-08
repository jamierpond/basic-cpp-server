#include <iostream>
#include <string>
#include <thread>
#include <format>

// Boost Beast headers
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>

// simdjson
// Local headers
#include "home.hpp"
#include "emily.hpp"
#include "shop.hpp"
#include "dashboard.hpp"
#include "tags.hpp"
#include "json_demo.hpp"
#include "contributions.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


// Port to listen on
constexpr static auto PORT = 3000;

// This function produces an HTTP response for the given request
template<class Body, class Allocator>
http::response<http::string_body>
handle_request(http::request<Body, http::basic_fields<Allocator>>&& req)
{
    // Returns a bad request response
    auto const bad_request = [&req](std::string_view why) {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found = [&req](std::string_view target) {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::format("404 Not Found: {}", target);
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error = [&req](std::string_view what) {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::format("500 Internal Server Error: {}", what);
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if (req.method() != http::verb::get)
        return bad_request("Unknown HTTP method");

    // Request path must be absolute and not contain ".."
    if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != beast::string_view::npos)
        return bad_request("Illegal request-target");

    // Build the response
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());

    // Extract the target path
    std::string path{req.target()};

    // Extract the session cookie if present
    std::string session_id;
    if (auto it = req.find("Cookie"); it != req.end()) {
        session_id = std::string(it->value());
    }

    try {
        // Handle different routes
        if (path == "/") {
            res.body() = home("/");
        } else if (path == "/emily") {
            res.body() = emily();
        } else if (path == "/shop") {
            res.body() = shop();
        } else if (path == "/dash") {
            res.body() = dashboard::dashboard();
        } else if (path == "/clicked") {
            using namespace pond;
            const auto clicked_response = pond::div {
                h2 { "hell yeah pretty cool" }
            };
            res.body() = clicked_response.render();
        } else if (path == "/json") {
            json_demo(path, res);
        } else if (path == "/contributions") {
            res.body() = contributions_page();
        } else if (path == "/api/contributions") {
            contributions_json(res);
        } else {
            return not_found(req.target());
        }
    } catch (const std::exception& e) {
        return server_error(e.what());
    }

    res.prepare_payload();
    return res;
}

// Handles an HTTP server connection
void do_session(tcp::socket socket)
{
    bool close = false;
    beast::error_code ec;

    // This buffer is required to persist across reads
    beast::flat_buffer buffer;

    for(;;)
    {
        // Read a request
        http::request<http::string_body> req;
        http::read(socket, buffer, req, ec);
        if(ec == http::error::end_of_stream)
            break;
        if(ec)
            return;

        // Send the response
        auto res = handle_request(std::move(req));
        close = res.need_eof();

        // Write the response
        http::write(socket, res, ec);
        if(ec)
            return;

        if(close)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            break;
        }
    }

    // Send a TCP shutdown
    auto _ = socket.shutdown(tcp::socket::shutdown_send, ec);
    // At this point the connection is closed gracefully
}

int main()
{
    try
    {
        auto const address = net::ip::make_address("0.0.0.0");

        // The io_context is required for all I/O
        net::io_context ioc{1};

        // The acceptor receives incoming connections
        tcp::acceptor acceptor{ioc, {address, PORT}};

        std::cout << std::format("Server listening on port {}...\n", PORT);

        for(;;)
        {
            // This will receive the new connection
            tcp::socket socket{ioc};

            // Block until we get a connection
            acceptor.accept(socket);

            // Launch the session, transferring ownership of the socket
            std::jthread{do_session, std::move(socket)}.detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << std::format("Error: {}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
