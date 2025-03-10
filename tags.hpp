#pragma once

#include <initializer_list>
#include <string>
#include <vector>

namespace html {

// String literal template for compile-time string handling
template <size_t N> struct StringLiteral {
  char value[N];
  constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }

  template <size_t M> constexpr StringLiteral(const StringLiteral<M> &other) {
    static_assert(M <= N, "StringLiteral too large");
    std::copy_n(other.value, M, value);
  }

};


// static_assert(from_string_view<1>("foo").value[0] == 'f');

template <StringLiteral TagName, StringLiteral ClassNames = "">
struct tag_base {
private:
  struct Attributes {
    std::string style, id, href, src;
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
  constexpr tag_base(const std::string (&str)[N]) {
    for (size_t i = 0; i < N; i++) {
      content += str[i];
    }
  }

  template <typename... T> constexpr tag_base(const T &...) {}

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

  constexpr auto with_src(const std::string &src) {
    auto copy = *this;
    copy.attributes.src = src;
    return copy;
  }

    template <typename... Args>
    constexpr tag_base(Args&&... args) {
        (add_child(std::forward<Args>(args)), ...);
    }

    template <typename T>
    constexpr void add_child(const T& child) {
        if constexpr (std::is_convertible_v<T, std::string>) {
            children.push_back(child);
        } else {
            children.push_back(child.render());
        }
    }

  constexpr operator std::string() const { return render(); }

  // this function is a messss but it's a contained mess :)
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

    if (!attributes.src.empty()) {
      s += " src='";
      s += attributes.src;
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


#define CREATE_TAG(Tag)                                                        \
  template <StringLiteral ClassName = "">                                      \
  struct Tag : tag_base<#Tag, ClassName> {                                     \
    using tag_base<#Tag, ClassName>::tag_base; /* Inherit constructors */      \
  };                                                                           \
                                                                               \
  template <StringLiteral ClassName = "">                                      \
  Tag(const std::string &) -> Tag<ClassName>;                                  \
                                                                               \
  template <StringLiteral ClassName = "", typename... T>                       \
  Tag(const T&&...) -> Tag<ClassName>;                                         \
                                                                                \
  template <typename... T>                                                     \
  Tag(T&&...) -> Tag<"">;                                                       \
                                                                                \
  template <StringLiteral ClassName = ""> Tag(const char *) -> Tag<ClassName>; \

CREATE_TAG(div)
CREATE_TAG(p)
CREATE_TAG(b)
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

static_assert(div{
                      p<"bg-red-500">{"foo"},
                      p{"foo"},
                  }
                  .render() ==
              "<div><p class='bg-red-500'>foo</p><p>foo</p></div>");

// static_assert(a{"Click me"}.with_href("https://example.com").render() ==
//               "<a href='https://example.com'>Click me</a>");

consteval auto home() {
  using namespace html;
  return div{
      {p{"This is a simple HTTP server written in C++ using only the standard "
         "library."},
       a<"bg-blue-500 text-white">{"This is a simple HTTP server written in "
                                   "C++ using only the standard library."}}};
}

static_assert(div{html::a{"This is a simple HTTP server written in C++ using "
                           "only the standard library."}}
                  .render() ==
              "<div><a>This is a simple HTTP server written in C++ using only "
              "the standard library.</a></div>");

static_assert(h1{"Hello, World!", a{"click me"}}.render() ==
              "<h1>Hello, World!<a>click me</a></h1>");

} // namespace html
