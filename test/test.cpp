#define CATCH_CONFIG_MAIN
#include "../src/tags.hpp"
#include <catch2/catch_all.hpp>
#include <iostream>

#define STATIC_TESTS 1
#if STATIC_TESTS
#include "static_tests.hpp"
#endif
#undef STATIC_TESTS

using namespace pond;

TEST_CASE("Hello, World!") {}

TEST_CASE("a with href") {
  auto got = pond::a{"Click me"}.with_href("https://example.com").render();
  std::cout << std::string{got} << std::endl;
  auto expexted = "<a href='https://example.com'>Click me</a>";
  REQUIRE(got == expexted);
}

TEST_CASE("meta with charset") {
  auto got = pond::meta{}.with("charset", "UTF-8").with("name", "viewport");
  auto expexted = "<meta charset='UTF-8' name='viewport'></meta>";
  REQUIRE(got.render() == expexted);
}
