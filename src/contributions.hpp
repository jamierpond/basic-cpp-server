#pragma once
#include <string>
#include <vector>
#include "layout.hpp"
#include "tags.hpp"

// Boost Beast headers for HTTP client
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

// simdjson for parsing
#include <simdjson.h>
namespace json = simdjson;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

struct Contribution {
    std::string date;
    int count;
    int level;
};

inline std::string fetch_contributions() {
    try {
        // Set up I/O context
        net::io_context ioc;

        // Resolve the host
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("github-contributions-api.jogruber.de", "443");

        // Set up SSL context
        ssl::context ctx(ssl::context::tlsv12_client);
        ctx.set_default_verify_paths();

        // Create SSL stream
        ssl::stream<beast::tcp_stream> stream(ioc, ctx);

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(!SSL_set_tlsext_host_name(stream.native_handle(), "github-contributions-api.jogruber.de")) {
            beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            throw beast::system_error{ec};
        }

        // Connect to host
        beast::get_lowest_layer(stream).connect(results);

        // Perform SSL handshake
        stream.handshake(ssl::stream_base::client);

        // Set up HTTP request
        http::request<http::string_body> req{http::verb::get, "/v4/jamierpond?y=last", 11};
        req.set(http::field::host, "github-contributions-api.jogruber.de");
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the request
        http::write(stream, req);

        // Receive the response
        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);

        // Get the response body
        std::string response_body = beast::buffers_to_string(res.body().data());

        // Shutdown the connection
        beast::error_code ec;
        auto _ = stream.shutdown(ec);
        if(ec == net::error::eof) {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec = {};
        }
        if(ec) {
            throw beast::system_error{ec};
        }

        return response_body;
    }
    catch(std::exception const& e) {
        return std::string("{\"error\": \"") + e.what() + "\"}";
    }
}

inline std::vector<Contribution> parse_contributions(const std::string& json_data) {
    std::vector<Contribution> result;

    try {
        json::ondemand::parser parser;
        json::padded_string padded_json(json_data);
        auto doc = parser.iterate(padded_json);

        // Parse the contributions array
        json::ondemand::array contributions = doc["contributions"].get_array();

        // Iterate through contributions and add to result
        for (auto contribution : contributions) {
            Contribution item;
            item.date = std::string(contribution["date"].get_string().value());
            item.count = contribution["count"].get_int64().value();
            item.level = contribution["level"].get_int64().value();
            result.push_back(item);
        }
    }
    catch(const json::simdjson_error& e) {
        // Handle parsing errors
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
    }

    return result;
}

inline std::string contributions_page() {
    // Fetch contributions data
    std::string json_data = fetch_contributions();
    std::vector<Contribution> contributions = parse_contributions(json_data);

    // Create HTML using pond
    using namespace pond;

    // Create table rows for contributions
    std::vector<std::string> rows;

    // Add header row
    rows.push_back(tr {
        th { "Date" },
        th { "Count" },
        th { "Level" }
    }.render());

    // Add data rows
    for (const auto& contrib : contributions) {
        rows.push_back(tr {
            td { contrib.date },
            td { std::to_string(contrib.count) },
            td { std::to_string(contrib.level) }
        }.render());
    }

    auto all_rows = [&rows] {
        std::string result;
        for (const auto& row : rows) {
            result += row;
        }
        return result;
    }();

    // Create the full page
    return pond::div {
        layout("GitHub Contributions",
            h1 { "GitHub Contributions" },
            p { "Contributions from GitHub for jamierpond over the last year" },
            pond::div{
              pond::table { all_rows }
            }
        )
    }.render();
}

// JSON API endpoint for raw contribution data
inline void contributions_json(http::response<http::string_body>& res) {
    // Fetch contributions data
    std::string json_data = fetch_contributions();

    // Set content type to JSON
    res.set(http::field::content_type, "application/json");

    // Return the JSON directly
    res.body() = json_data;
}
