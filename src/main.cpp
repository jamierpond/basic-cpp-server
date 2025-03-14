#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>

#include "home.hpp"
#include "emily.hpp"
#include "shop.hpp"
#include "dashboard.hpp"
#include "js/tailwind_gz.hpp"

constexpr static auto PORT = 3000;

constexpr auto generate_sitemap_xml() {
}

constexpr auto create_http_response_from_html(const std::string& body) {
    std::string doctype = "<!DOCTYPE html>";

    auto html = pond::html{
        pond::head{
            pond::meta{}.with("charset", "UTF-8"),
            pond::meta{}.with("name", "viewport")
                        .with("content", "width=device-width, initial-scale=1.0"),
            pond::script{}.with_src("/tailwind"),
            pond::title{"Jamie Pond's C++ HTTP Server"}
        },
        pond::body{body}
    }.render();

    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: " + std::to_string(html.size()) + "\r\n"
           "Connection: close\r\n"
           "\r\n" + html;
}

constexpr auto get_gzipped_header(int size, const std::string& content_type = "text/javascript") {
  //do cache-control: no-transform
  return "HTTP/1.1 200 OK\r\n"
         "Content-Type: " + content_type + "\r\n"
         "Content-Encoding: gzip\r\n"
         "Content-Length: " + std::to_string(size) + "\r\n"
         "Connection: close\r\n"
         "Cache-Control: no-transform\r\n"
         "encodeBody: manual\r\n"
         "\r\n";
}


std::pair<std::unique_ptr<uint8_t[]>, size_t> load_gzipped_file(const std::string& file_path) {
    // Open the file in binary mode and move to the end to get the file size
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate memory for the file data
    auto buffer = std::make_unique<uint8_t[]>(file_size);

    // Read file content into the buffer
    if (!file.read(reinterpret_cast<char*>(buffer.get()), file_size)) {
        throw std::runtime_error("Failed to read file: " + file_path);
    }

    return { std::move(buffer), file_size };
}

struct Endpoint {
  std::string path;
  std::string repsonshe;
};

// Extract the requested path from the HTTP request
constexpr std::string get_request_path(const std::string& request) {
    size_t start = request.find("GET ");
    if (start == std::string::npos) return "/";
    start += 4; // Move past "GET "

    size_t end = request.find(" ", start);
    if (end == std::string::npos) return "/";

    return request.substr(start, end - start);
}

constexpr std::string get_session_id(const std::string& request) {
    size_t start = request.find("Cookie: ");
    if (start == std::string::npos) return "";
    start += 8; // Move past "Cookie: "

    size_t end = request.find(";", start);
    if (end == std::string::npos) return "";
    return request.substr(start, end - start);
}

static_assert(get_request_path("GET / HTTP/1.1") == "/");
static_assert(get_request_path("GET /about HTTP/1.1") == "/about");
static_assert(get_request_path("GET /contact HTTP/1.1") == "/contact");
static_assert(get_request_path("GET /index/lemon HTTP/1.1") == "/index/lemon");



int main() {
    int server_fd, new_socket;
    sockaddr_in address{};
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed\n";
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed\n";
        close(server_fd);
        return 1;
    }

    auto send_page = [&new_socket] (const auto& content) {
      auto c = create_http_response_from_html(content);
      send(new_socket, c.c_str(), c.size(), 0);
      close(new_socket);
    };

    auto header = get_gzipped_header(TAILWIND_GZ_DATA.size());
    auto* tw_data = TAILWIND_GZ_DATA.data();
    auto tw_size = TAILWIND_GZ_DATA.size();
    auto send_tailwind = [&] {
          std::cout << "Serving tailwind.js\n";
          send(new_socket, header.c_str(), header.size(), 0);
          send(new_socket, tw_data, tw_size, 0);
          close(new_socket);
    };

    const auto content_lookup = std::unordered_map<std::string, std::function<void()>> {
        {"/", [&] { send_page(home("/")); }},
        {"/emily", [&] { send_page(emily()); }},
        {"/shop", [&] { send_page(shop()); }},
        {"/dash", [&] { send_page(dashboard::dashboard()); }},
        {"/tailwind", send_tailwind}
    };

    auto generate_sitemap = [](auto content) {
      auto sitemap = pond::url{};

    }


    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (new_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        read(new_socket, buffer, sizeof(buffer));

        auto request = std::string{buffer};
        auto path = get_request_path(request);
        auto response = std::string{};
        auto session_id = get_session_id(request);

        try {
            content_lookup.at(path)();
        } catch (const std::out_of_range& e) {
            std::cerr << "404 Not Found: " << path << '\n';
            std::string body = "404 Not Found";
            response = create_http_response_from_html(body);
        } catch (const std::exception& e) {
            std::cerr << "500 Internal Server Error: " << e.what() << '\n';
            std::string body = "500 Internal Server Error";
            response = create_http_response_from_html(body);
        }

        send(new_socket, response.c_str(), response.size(), 0);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}

