cc_library(
    name = "src_lib",
    hdrs = glob(["src/*.hpp"]),  # Include all header files
    srcs = glob(["src/*.cpp"]),  # Include all .cpp files except main.cpp
    includes = ["src"],  # Allow headers to be found
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "main",
    srcs = ["src/main.cpp"],
    deps = [":src_lib"],  # Link with the library that includes headers
    copts = ["-std=c++23"],
)

