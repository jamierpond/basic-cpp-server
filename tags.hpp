#pragma once

#include <algorithm>
#include <string>
#include <type_traits>
#include <vector>

namespace pond {

template <size_t N, typename T>
struct static_string {
  std::array<T, N> data;
  size_t size = 0;

  constexpr static_string() = default;
  constexpr explicit static_string(const T (&str)[N]) {
    std::copy_n(str, N, data.begin());
    size = N;
  }

  constexpr static_string(const T *str, size_t len) {
    std::copy_n(str, len, data.begin());
    size = len;
  }

  constexpr static_string(const T *str) {
    size = 0;
    // eek - that's a scary while loop
    while (*str) {
      data[size++] = *str++;
    }
  }

  constexpr static_string(const static_string &other) {
    std::copy_n(other.data.begin(), other.size, data.begin());
    size = other.size;
  }

  constexpr static_string &operator=(const static_string &other) {
    std::copy_n(other.data.begin(), other.size, data.begin());
    size = other.size;
    return *this;
  }

  constexpr static_string &operator+=(const T &c) {
    data[size++] = c;
    return *this;
  }

  constexpr static_string &operator+=(const T (&str)[N]) {
    std::copy_n(str, N, data.begin() + size);
    size += N;
    return *this;
  }

  constexpr static_string &operator+=(const static_string &other) {
    std::copy_n(other.data.begin(), other.size, data.begin() + size);
    size += other.size;
    return *this;
  }

  constexpr size_t length() const { return size; }
  constexpr size_t max_size() const { return N; }
  constexpr bool empty() const { return size == 0; }
  constexpr const T *c_str() const { return data.data(); }

  constexpr T &operator[](size_t i) { return data[i]; }

  constexpr const T &operator[](size_t i) const { return data[i]; }

  constexpr bool operator==(const static_string &other) const {
    return size == other.size &&
           std::equal(data.begin(), data.begin() + size, other.data.begin());
  }

  template <size_t M>
  constexpr bool operator==(const char (&str)[N]) {
    return size == M && std::equal(data.begin(), data.begin() + size, str);
  }

  // castable to std::string
  operator std::string() const { return std::string(data.begin(), data.begin() + size); }

  constexpr bool operator!=(const static_string &other) const {
    return *this != other;
  }

};

// String literal template for compile-time string handling
template <size_t N> struct StringLiteral {
  char value[N];
  constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }
  template <size_t M> constexpr StringLiteral(const StringLiteral<M> &other) {
    static_assert(M <= N, "StringLiteral too large");
    std::copy_n(other.value, M, value);
  }
};

template <StringLiteral TagName, StringLiteral ClassNames = "",
          typename StringImpl = std::string>
struct tag_base {
private:
  struct Attributes {
    StringImpl style{}, id{}, href{}, src{};
  };
  Attributes attributes{};
  StringImpl content{};
  std::vector<StringImpl> children{};

public:
  // Constructors for text content
  constexpr tag_base() = default;

  constexpr tag_base(const char *str) : content(str) {}
  constexpr tag_base(const StringImpl &s) : content(s) {}

  // Single-argument constructor: if it's string-like, use it as content;
  // otherwise, assume it's another tag and call .render()
  template <typename T> constexpr tag_base(const T &child) {
    if constexpr (std::is_convertible_v<T, StringImpl>) {
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
  constexpr auto with_href(const StringImpl &href) const {
    tag_base copy = *this;
    copy.attributes.href = href;
    return copy;
  }

  constexpr auto with_src(const StringImpl &src) const {
    tag_base copy = *this;
    copy.attributes.src = src;
    return copy;
  }

  // Helper to add a child tag or string
  template <typename T> constexpr void add_child(const T &child) {
    if constexpr (std::is_convertible_v<T, StringImpl>) {
      children.push_back(child);
    } else {
      children.push_back(child.render());
    }
  }

  // Render function
  constexpr StringImpl render() const {
    StringImpl s;
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
  template <StringLiteral ClassName = "", typename StringImpl = std::string>   \
  struct Tag : tag_base<#Tag, ClassName> {                                     \
    using tag_base<#Tag, ClassName>::tag_base;                                 \
  };                                                                           \
                                                                               \
  template <StringLiteral ClassName = "", typename StringImpl = std::string>   \
  Tag(const StringImpl &) -> Tag<ClassName>;                                   \
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
CREATE_TAG(html)
CREATE_TAG(span)
CREATE_TAG(body)
CREATE_TAG(head)
CREATE_TAG(title)
CREATE_TAG(meta)
CREATE_TAG(link)
CREATE_TAG(script)
CREATE_TAG(style)
CREATE_TAG(header)
CREATE_TAG(footer)

CREATE_TAG(ul)
CREATE_TAG(ol)
CREATE_TAG(li)
CREATE_TAG(nav)
CREATE_TAG(section)
CREATE_TAG(article)
CREATE_TAG(main)
CREATE_TAG(aside)
CREATE_TAG(table)
CREATE_TAG(tr)
CREATE_TAG(th)
CREATE_TAG(td)
CREATE_TAG(thead)
CREATE_TAG(tbody)
CREATE_TAG(tfoot)
CREATE_TAG(col)


CREATE_TAG(a)
CREATE_TAG(img)
CREATE_TAG(input)
CREATE_TAG(button)
CREATE_TAG(form)

} // namespace html
