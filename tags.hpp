#pragma once

#include <algorithm>
#include <string>
#include <type_traits>
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

template <StringLiteral TagName, StringLiteral ClassNames = "">
struct tag_base {
private:
  struct Attributes {
    std::string style{}, id{}, href{}, src{};
  };
  Attributes attributes;
  std::string content;
  std::vector<std::string> children;

public:
  // Constructors for text content
  constexpr tag_base() = default;

  constexpr tag_base(const char *str) : content(str) {}
  constexpr tag_base(const std::string &s) : content(s) {}

  // Single-argument constructor: if it's string-like, use it as content;
  // otherwise, assume it's another tag and call .render()
  template <typename T> constexpr tag_base(const T &child) {
    if constexpr (std::is_convertible_v<T, std::string>) {
      content = child;
    } else {
      children.push_back(child.render());
    }
  }

  // Multi-argument constructor: build children from each argument
  template <typename First, typename... Rest>
  constexpr tag_base(const First &first, const Rest &...rest)
      : tag_base(first) {
    (add_child(rest), ...);
  }

  // Copy constructor
  constexpr tag_base(const tag_base &other)
      : attributes(other.attributes), content(other.content),
        children(other.children) {}

  // Accessors that return a new copy with attribute set
  constexpr auto with_href(const std::string &href) const {
    tag_base copy = *this;
    copy.attributes.href = href;
    return copy;
  }

  constexpr auto with_src(const std::string &src) const {
    tag_base copy = *this;
    copy.attributes.src = src;
    return copy;
  }

  // Helper to add a child tag or string
  template <typename T> constexpr void add_child(const T &child) {
    if constexpr (std::is_convertible_v<T, std::string>) {
      children.push_back(child);
    } else {
      children.push_back(child.render());
    }
  }

  // Render function
  constexpr std::string render() const {
    std::string s;
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

// Helper macro to define specific tags
#define CREATE_TAG(Tag)                                                        \
  template <StringLiteral ClassName = "">                                      \
  struct Tag : tag_base<#Tag, ClassName> {                                     \
    using tag_base<#Tag, ClassName>::tag_base;                                 \
  };                                                                           \
                                                                               \
  template <StringLiteral ClassName = "">                                      \
  Tag(const std::string &) -> Tag<ClassName>;                                  \
                                                                               \
  template <StringLiteral ClassName = "", typename... T>                       \
  Tag(const T &...) -> Tag<ClassName>;                                         \
                                                                               \
  template <typename... T> Tag(T &&...) -> Tag<"">;                            \
                                                                               \
  template <StringLiteral ClassName = ""> Tag(const char *) -> Tag<ClassName>;

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

} // namespace html
