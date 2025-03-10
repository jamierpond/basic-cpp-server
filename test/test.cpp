#define CATCH_CONFIG_MAIN
#include "../tags.hpp"
#include <catch2/catch_all.hpp>

#include <iostream>

using namespace html;

static_assert(p<"bg-red-500">{"foo"}.render() ==
              "<p class='bg-red-500'>foo</p>");

static_assert(tag_base<"div", "cn">{
                  p{"foo"},
                  p<"hello">{"bar"},
              }
                  .render() ==
              "<div class='cn'><p>foo</p><p class='hello'>bar</p></div>");

static_assert(html::div{
                      p<"bg-red-500">{"foo"},
                      p{"foo"},
                  }
                  .render() ==
              "<div><p class='bg-red-500'>foo</p><p>foo</p></div>");

static_assert(a{"Click me"}.with_href("https://example.com").render() == "<a href='https://example.com'>Click me</a>");

consteval auto home() {
  using namespace html;
  return html::div {
      p{"This is a simple HTTP server written in C++ using only the standard library."},
      a<"bg-blue-500 text-white">{"This is a simple HTTP server written in C++ using only the standard library."}
  };
}

static_assert(html::div{
  html::a{"This is a simple HTTP server written in C++ using only the standard library."}
}.render() == "<div><a>This is a simple HTTP server written in C++ using only the standard library.</a></div>");

TEST_CASE("Hello, World!") {}


TEST_CASE("a with href") {
  auto got = html::a{"Click me"}.with_href("https://example.com").render();
  std::cout << got << std::endl;
  auto expexted = "<a href='https://example.com'>Click me</a>";
  REQUIRE(got == expexted);
}


