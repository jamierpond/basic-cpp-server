cmake_minimum_required(VERSION 3.14)
project(PondAudio VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

if(APPLE)
  add_compile_options(-fexperimental-library)
endif()


# Define BOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT to handle C++20/23 compatibility
add_compile_definitions(BOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT)
add_compile_definitions(BOOST_ASIO_DISABLE_STD_RESULT_OF)

if(USE_ASAN)
    message(STATUS "Enabling Address Sanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(USE_TSAN)
    message(STATUS "Enabling Thread Sanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(USE_MSAN)
    message(STATUS "Enabling Memory Sanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=memory")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=memory")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find required Boost components
find_package(Boost REQUIRED COMPONENTS system thread)

# Check if Boost was found
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif()

message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")

# Find required OpenSSL package
find_package(OpenSSL REQUIRED)

# Add simdjson as a subdirectory
add_subdirectory(third-party/simdjson)

# Main executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Link against Boost, OpenSSL, and simdjson
target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::system
    Boost::thread
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
    simdjson
)

# Fetch and configure Catch2
Include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Tests
enable_testing()
add_executable(tests test/test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tests)
