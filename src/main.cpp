#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "tags.hpp"
#include <string>
#include <unordered_map>
#include <functional>

#include "home.hpp"
#include "emily.hpp"
#include "shop.hpp"
#include "dashboard.hpp"
#include "js/tailwind_gz.hpp"

constexpr static auto PORT = 3000;


constexpr auto http_header = [](auto size, const std::string& content_type = "text/html", const std::string& extra_headers = "") {
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: " + content_type + "\r\n"
           "Content-Length: " + size + "\r\n"
           "Connection: close\r\n"
           + extra_headers + "\r\n";
};

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

    return http_header(doctype + html);
}

constexpr auto get_gzipped_header(int size, const std::string& content_type = "text/javascript") {
  //do cache-control: no-transform
  return http_header("", content_type, "Content-Encoding: gzip\r\nContent-Length: " + std::to_string(size));
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
  std::string_view path, content;
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

    auto send_page = [&new_socket] (const auto& content, const auto& mime_type = "text/html") {
      auto c = create_http_response_from_html(content, mime_type);
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

    auto generate_sitemap = [](auto& content_lookup) {
      auto now_string = __DATE__;
      auto now = std::string{now_string};

      // flex for external polymorphism
      std::vector<pond::ToString<std::string>*> urls{};
      for (const auto& [path, _] : content_lookup) {
        urls.push_back(new pond::url{path, now, "daily", "1.0"});
      }

      auto str_urls = std::string{};
      for (const auto& url : urls) {
        // could have just done this in one step,
        // but wanted to show the flexibility
        str_urls += url->render();
      }
      return pond::urlset{
        str_urls
      }.render();
    };


    const auto content_lookup = std::unordered_map<std::string, std::function<void()>> {
        {"/", [&] { send_page(home("/"), "text/html"); }},
        {"/emily", [&] { send_page(emily(), "text/html"); }},
        {"/shop", [&] { send_page(shop(), "text/html"); }},
        {"/dash", [&] { send_page(dashboard::dashboard(), "text/html"); }},
        {"/tailwind", send_tailwind},
    };

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

        if (path == "/sitemap.xml") {
          auto sm = generate_sitemap(content_lookup);
          auto c = create_http_response_from_html(sm, "application/xml");
          send(new_socket, response.c_str(), response.size(), 0);
          close(new_socket);
          continue;
        }

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

