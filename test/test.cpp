#define CATCH_CONFIG_MAIN
#include "../tags.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Hello, World!") {
  auto got = html::a<"text-blue-500">{{"a", html::span{"foo"}}}.render();
  auto expexted = "<a class='text-blue-500'>a<span>foo</span></a>";
  REQUIRE(got == expexted);
}
