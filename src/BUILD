cc_binary(
    name = "server",
    srcs = ["main.cpp"],
    deps = [":server_lib"],
    copts = ["-std=c++23"],
)

cc_library(
    name = "server_lib",
    hdrs = [
        "dashboard.hpp",
        "emily.hpp",
        "encoding.hpp",
        "home.hpp",
        "layout.hpp",
        "shop.hpp",
        "static_string.hpp",
        "tags.hpp",
    ],
    includes = ["../third-party/static_string"],
    copts = ["-std=c++23"],
)

# Sanitizer builds
cc_binary(
    name = "server_asan",
    srcs = ["main.cpp"],
    deps = [":server_lib"],
    copts = ["-std=c++23", "-fsanitize=address"],
    linkopts = ["-fsanitize=address"],
)

cc_binary(
    name = "server_tsan",
    srcs = ["main.cpp"],
    deps = [":server_lib"],
    copts = ["-std=c++23", "-fsanitize=thread"],
    linkopts = ["-fsanitize=thread"],
)

cc_binary(
    name = "server_msan",
    srcs = ["main.cpp"],
    deps = [":server_lib"],
    copts = ["-std=c++23", "-fsanitize=memory"],
    linkopts = ["-fsanitize=memory"],
)