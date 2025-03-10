#pragma once

#include <initializer_list>
#include <string>
#include <vector>

namespace html {

// String literal template for compile-time string handling
template <size_t N> struct StringLiteral {
  char value[N];
  constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }
};

template <StringLiteral TagName, StringLiteral ClassNames = "">
struct tag_base {
private:
  struct Attributes {
    std::string style, id, href;
  };
  Attributes attributes;
  std::string content;
  std::vector<std::string> children;

public:
  // Default constructor for empty tags
  constexpr tag_base() = default;

  // Constructor for text content
  constexpr tag_base(const std::string &str) : content(str) {}

  template <size_t N>
  constexpr tag_base(const std::string (&str)[N]) : content(str) {}

  // Constructor for text content from string literal
  constexpr tag_base(const char *str) : content(str) {}

  // Copy constructor
  constexpr tag_base(const tag_base &other) {
    content = other.content;
    children = other.children;
    attributes = other.attributes;
  }

  // Generic constructor for a single child
  template <typename T> constexpr tag_base(const T &child) {
    if constexpr (std::is_convertible_v<T, std::string>) {
      content = child;
    } else {
      children.push_back(child.render());
    }
  }

  constexpr auto with_href(const std::string &href) {
    auto copy = *this;
    copy.attributes.href = href;
    return copy;
  }

  // Accepts an initializer list of valid child types
  template <typename T>
  constexpr tag_base(std::initializer_list<T> init_children) {
    for (const auto &child : init_children) {
      if constexpr (std::is_convertible_v<T, std::string>) {
        children.push_back(child);
      } else {
        children.push_back(child.render());
      }
    }
  }

  // Constructor for initializer list (multiple children)
  constexpr tag_base(std::initializer_list<std::string> init_children) {
    for (const auto &child : init_children) {
      children.push_back(child);
    }
  }

  constexpr operator std::string() const { return render(); }

  constexpr std::string render() const {
    std::string s{};

    s += "<";
    s += TagName.value;

    if constexpr (sizeof(ClassNames.value) > 1) {
      s += " class='";
      s += ClassNames.value;
      s += "'";
    }

    if (!attributes.style.empty()) {
      s += " style='";
      s += attributes.style;
      s += "'";
    }

    if (!attributes.id.empty()) {
      s += " id='";
      s += attributes.id;
      s += "'";
    }

    if (!attributes.href.empty()) {
      s += " href='";
      s += attributes.href;
      s += "'";
    }

    s += ">";

    if (!content.empty()) {
      s += content;
    }

    for (const auto &child : children) {
      s += child;
    }

    s += "</";
    s += TagName.value;
    s += ">";

    return s;
  }
};

// using p = tag_base<"p", ClassName>;

#define CREATE_TAG(Tag)                                                        \
  template <StringLiteral ClassName = "">                                      \
  struct Tag : tag_base<#Tag, ClassName> {};                                   \
                                                                               \
  template <StringLiteral ClassName = "">                                      \
  Tag(std::initializer_list<std::string>) -> Tag<ClassName>;                   \
                                                                               \
  template <StringLiteral ClassName = "">                                      \
  Tag(const std::string &) -> Tag<ClassName>;                                  \
                                                                               \
  template <StringLiteral ClassName = ""> Tag(const char *) -> Tag<ClassName>;

CREATE_TAG(div)
CREATE_TAG(p)
CREATE_TAG(h1)
CREATE_TAG(h2)
CREATE_TAG(h3)
CREATE_TAG(h4)
CREATE_TAG(h5)
CREATE_TAG(h6)
CREATE_TAG(span)

CREATE_TAG(a)
CREATE_TAG(img)
CREATE_TAG(input)
CREATE_TAG(button)
CREATE_TAG(form)

static_assert(p<"bg-red-500">{"foo"}.render() ==
              "<p class='bg-red-500'>foo</p>");

static_assert(tag_base<"div", "cn">{
                  p{"foo"},
                  p<"hello">{"bar"},
              }
                  .render() ==
              "<div class='cn'><p>foo</p><p class='hello'>bar</p></div>");

static_assert(div{{
                      p<"bg-red-500">{"foo"},
                      p{"foo"},
                  }}
                  .render() ==
              "<div><p class='bg-red-500'>foo</p><p>foo</p></div>");

static_assert(a{"Click me"}.with_href("https://example.com").render() == "<a href='https://example.com'>Click me</a>");

consteval auto home() {
  using namespace html;
  return div {{
      p{"This is a simple HTTP server written in C++ using only the standard library."},
      a<"bg-blue-500 text-white">{"This is a simple HTTP server written in C++ using only the standard library."}
  }};
}

static_assert(div{{
  html::a{"This is a simple HTTP server written in C++ using only the standard library."}
}}.render() == "<div><a>This is a simple HTTP server written in C++ using only the standard library.</a></div>");


} // namespace h
