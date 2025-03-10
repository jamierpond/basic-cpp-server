#define CATCH_CONFIG_MAIN
#include "../tags.hpp"
#include <catch2/catch_all.hpp>

#include <iostream>

TEST_CASE("Hello, World!") {
  auto got = html::a<"text-blue-500">{{"a", html::span{"foo"}}}.render();
  auto expexted = "<a class='text-blue-500'>a<span>foo</span></a>";
  REQUIRE(got == expexted);
}

// auto got = html::a{"Click me"}.with_href("https://example.com").render();

TEST_CASE("a with href") {
  auto got = html::a{"Click me"}.with_href("https://example.com").render();
  std::cout << got << std::endl;
  auto expexted = "<a href='https://example.com'>Click me</a>";
  REQUIRE(got == expexted);
}


