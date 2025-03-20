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

TEST_CASE("svg") {
  auto svg = pond::svg {
    pond::path{}.with("d", "smth-cool").with("fill", "#lemon"),
    pond::path{}.with("d", "smth-cool").with("fill", "#lemon")
  }
  .with("version", "1.1")
  .with("xmlns", "http://www.w3.org/2000/svg").render();

  auto expexted = "<svg version='1.1' xmlns='http://www.w3.org/2000/svg'><path d='smth-cool' fill='#lemon'></path><path d='smth-cool' fill='#lemon'></path></svg>";
  REQUIRE(svg == expexted);
}


TEST_CASE("imgimg") {
  auto svg = pond::svg {
    pond::path{}.with("d", "smth-cool").with("fill", "#lemon"),
    pond::path{}.with("d", "smth-cool").with("fill", "#lemon")
  }
  .with("version", "1.1")
  .with("xmlns", "http://www.w3.org/2000/svg").render();

  auto expexted = "<svg version='1.1' xmlns='http://www.w3.org/2000/svg'><path d='smth-cool' fill='#lemon'></path><path d='smth-cool' fill='#lemon'></path></svg>";
  REQUIRE(svg == expexted);


  auto img = pond::div<"w-8 h-8"> {
    pond::img{}.with("src", "data:image/svg+xml;utf8," + svg).with("alt", "logo")
  };

  auto expexted_img = "<div class='w-8 h-8'><img src='data:image/svg+xml;utf8,<svg version='1.1' xmlns='http://www.w3.org/2000/svg'><path d='smth-cool' fill='#lemon'></path><path d='smth-cool' fill='#lemon'></path></svg>' alt='logo'></img></div>";
  REQUIRE(img.render() == expexted_img);
}

