#pragma once
#include <string>
#include "layout.hpp"

// Boost Beast headers
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

#include <simdjson.h>
namespace json = simdjson;

constexpr auto json_demo = [](const std::string& path, http::response<http::string_body>& res) {
            json::ondemand::parser parser;
            // Create a simple JSON document
            std::string_view json_string = R"({
                "name": "Example JSON",
                "values": [1, 2, 3, 4, 5],
                "object": {
                    "key1": "value1",
                    "key2": "value2"
                }
            })";

            // Parse the JSON
            json::padded_string padded_json(json_string);
            auto doc = parser.iterate(padded_json);

            // Access values from the JSON
            std::string_view name = doc["name"].get_string().value();
            int64_t first_value = doc["values"].at(0).get_int64().value();
            std::string_view key1 = doc["object"]["key1"].get_string().value();

            // Use pond to create an HTML response
            using namespace pond;
            const auto json_response = pond::div {
              layout("JSON Parsing Demo",
                h2 { "JSON Parsing Demo" },
                p { std::format("Name: {}", name) },
                p { std::format("First value: {}", first_value) },
                p { std::format("Object key1: {}", key1) },
                h3 { "Original JSON:" },
                pre { std::string(json_string) }
                )
            };
            res.body() = json_response.render();
        };

