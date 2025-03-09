#include <cstddef>
#include <iostream>
#include <cstring>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 3000


namespace tags {

struct p_data {
  std::string_view value;
  std::string_view class_name = "";
  constexpr static auto tag_name = "p";
};

struct Renderable {
  constexpr virtual std::string render() const = 0;
};

struct p : Renderable {
  constexpr p (std::string_view value, std::string_view class_name = "") : data{value, class_name} {}

  p_data data;
  constexpr std::string render() const override {
    using s = std::string;
    auto
        tag = s{data.tag_name},
        class_name = s{data.class_name},
        value = s{data.value};

    return "<" + tag + " class=\"" + class_name + "\">" + value + "</" + tag + ">";
  }
};

static_assert(p{"Hello, World!", "bg-red-100"}.render() == "<p class=\"bg-red-100\">Hello, World!</p>");


template <size_t N>
struct div : Renderable {
  // Store pointers/references to Renderable objects instead
  div(std::array<std::reference_wrapper<const Renderable>, N> children) : children{children} {}

  // Default constructor for the empty case
  div() {}

  std::array<std::reference_wrapper<const Renderable>, N> children;

  std::string render() const override {
    using s = std::string;
    auto tag = s{"div"};
    s children_str = "";
    for (const auto& child : children) {
      children_str += child.get().render();  // Note the .get() for reference_wrapper
    }
    return "<" + tag + " class=\"\">" + children_str + "</" + tag + ">";
  }
};

static_assert(div<2>{{
    p{"Hello, World!"},
    p{"Hello, World!"}
}}.render() == "<div class=\"\"><p class=\"\">Hello, World!</p><p class=\"\">Hello, World!</p></div>");



};





int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Configure address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed\n";
        close(server_fd);
        return 1;
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed\n";
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        // Accept connection
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (new_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        // Read request (not actually parsing it)
        read(new_socket, buffer, sizeof(buffer));
        std::cout << "Received request:\n" << buffer << "\n";

        // Send response
        const char* response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Hello, World!";

        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}

