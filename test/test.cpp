#define CATCH_CONFIG_MAIN
#include "../tags.hpp"
#include <catch2/catch_all.hpp>

#include <iostream>

using namespace pond;

static_assert(pond::p <"text-blue-500">{"p"}.render() == "<p class='text-blue-500'>p</p>");
static_assert(pond::p{"foo"}.render() == "<p>foo</p>");

static_assert(p<"bg-red-500">{"foo"}.render() ==
              "<p class='bg-red-500'>foo</p>");

static_assert(tag_base<"div", "cn">{
                  p{"foo"},
                  p<"hello">{"bar"},
              }
                  .render() ==
              "<div class='cn'><p>foo</p><p class='hello'>bar</p></div>");

static_assert(pond::div{
                      p<"bg-red-500">{"foo"},
                      p{"foo"},
                  }
                  .render() ==
              "<div><p class='bg-red-500'>foo</p><p>foo</p></div>");

static_assert(a{"Click me"}.with_href("https://example.com").render() == "<a href='https://example.com'>Click me</a>");

consteval auto home() {
  using namespace pond;
  return pond::div {
      p{"This is a simple HTTP server written in C++ using only the standard library."},
      a<"bg-blue-500 text-white">{"This is a simple HTTP server written in C++ using only the standard library."}
  };
}

static_assert(pond::div{
  pond::a{"This is a simple HTTP server written in C++ using only the standard library."}
}.render() == "<div><a>This is a simple HTTP server written in C++ using only the standard library.</a></div>");

static_assert(pond::p <"text-blue-500">{"p"}.render() == "<p class='text-blue-500'>p</p>");
static_assert(pond::p{"foo"}.render() == "<p>foo</p>");

TEST_CASE("Hello, World!") {}


TEST_CASE("a with href") {
  auto got = pond::a{"Click me"}.with_href("https://example.com").render();
  std::cout << std::string{got} << std::endl;
  auto expexted = "<a href='https://example.com'>Click me</a>";
  REQUIRE(got == expexted);
}


