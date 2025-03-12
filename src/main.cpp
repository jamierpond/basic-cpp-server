#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
// oss stream
#include <sstream>
#include <string>

#include "home.hpp"
#include "emily.hpp"
#include "shop.hpp"
#include "dashboard.hpp"

constexpr static auto PORT = 3000;

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

constexpr auto get_gzipped_header(int size) {
  return "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html\r\n"
          "Content-Encoding: gzip\r\n"
          "Content-Length: " + std::to_string(size) + "\r\n"
          "Connection: close\r\n"
          "\r\n";
}

#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>

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

std::string get_gzipped_response(const std::string& gzipped) {
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: text/plain\r\n"
             << "Content-Encoding: gzip\r\n"
             << "Content-Length: " << gzipped.size() << "\r\n"
             << "Vary: Accept-Encoding\r\n"
             << "Connection: close\r\n"
             << "\r\n"
             << gzipped;
    return response.str();
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
    struct sockaddr_in address;
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

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (new_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        read(new_socket, buffer, sizeof(buffer));
        std::string request(buffer);
        // std::cout << "Received request:\n" << request << "\n";

        std::string path = get_request_path(request);
        std::string response;

        // get session id from cookie

        auto session_id = get_session_id(request);
        std::cout << "Session ID: " << session_id << "\n";

        if (path == "/") {
            response = create_http_response_from_html(home(path));
        }

        else if (path == "/emily") {
            response = create_http_response_from_html(emily());
        }
        else if (path == "/shop") {
            response = create_http_response_from_html(shop());
        }
        else if (path == "/tailwind") {
            std::cout << "Serving tailwind.js\n";
            auto [tailwind_gzipped, tailwind_gzipped_size] = load_gzipped_file("/home/jamie/projects/basic-cpp-server/src/js/tailwind.js.gz");
            auto header = get_gzipped_header(tailwind_gzipped_size);
            send(new_socket, header.c_str(), header.size(), 0);
            send(new_socket, tailwind_gzipped.get(), tailwind_gzipped_size, 0);
            close(new_socket);
            continue;
        }

        else if (path == "/dash") {
            response = create_http_response_from_html(dashboard::dashboard());
        }
        else {
            response = create_http_response_from_html(home(path));
        }

        send(new_socket, response.c_str(), response.size(), 0);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}

