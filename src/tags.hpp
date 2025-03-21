#pragma once

#include <algorithm>
#include <array>
#include <string>
#include <type_traits>
#include <vector>

namespace hx {
struct command { constexpr static std::string_view cmd = "";};
struct get : command { constexpr static std::string_view cmd = "hx-get";};
struct post : command { constexpr static std::string_view cmd = "hx-post";};
struct swap : command { constexpr static std::string_view cmd = "hx-swap";};
} // namespace hx


namespace pond {

// String literal template for compile-time string handling
template <size_t N> struct StringLiteral {
  char value[N];
  constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }
  template <size_t M> constexpr StringLiteral(const StringLiteral<M> &other) {
    static_assert(M <= N, "StringLiteral too large");
    std::copy_n(other.value, M, value);
  }
};

template <hx::command Command, StringLiteral Value>
struct hx_pair {
  std::string_view value = std::string_view{Value};
  constexpr static std::string_view cmd = Command.cmd;
};

// simple struct to hold the attributes with helpers
template <typename Key, typename Value, size_t N> struct KvStaticArray {
  std::array<std::pair<Key, Value>, N> data{};
  size_t size = 0;
  constexpr void add(const Key &key, const Value &value) {
    // don't oopsie-daisy the size
    data[size++] = {key, value};
  }
};

// with_href, with_id, with_src
#define WITH_FOO(Foo)                                                          \
  template <typename T> constexpr auto with_##Foo(const T &t) const {          \
    return with(#Foo, t);                                                      \
  }

enum class ClosingOption { Full, SelfClosing, Void };

template <StringLiteral TagName, StringLiteral ClassNames = "",
          ClosingOption Closing = ClosingOption::Full,
          typename StringImpl = std::string>
struct tag_base {
  KvStaticArray<StringImpl, StringImpl, 10> attributes{};

  constexpr static bool ContentDisallowed = Closing != ClosingOption::Full;
  StringImpl content{};
  std::vector<StringImpl> children{};

  // Constructors for text content
  constexpr tag_base() = default;

  constexpr tag_base(const char *str) : content(str) {
    static_assert(!ContentDisallowed, "Content disallowed for full tags");
  }

  constexpr tag_base(const StringImpl &s) : content(s) {
    static_assert(!ContentDisallowed, "Content is only allowed for full tags");
  }

  // constructor for variadic hx_pairs
  template <typename... hx_pair>
  constexpr tag_base(const hx_pair &...hx_pairs) {
    (attributes.add(hx_pairs.first.cmd, hx_pairs.second), ...);
  }


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

  constexpr static bool is_self_closing() {
    return Closing == ClosingOption::SelfClosing;
  }

  constexpr static bool is_void() { return Closing == ClosingOption::Void; }

  // Copy constructor
  constexpr tag_base(const tag_base &other)
      : attributes(other.attributes), content(other.content),
        children(other.children) {}

  // Accessors that return a new copy with attribute set
  WITH_FOO(class_)
  WITH_FOO(style)
  WITH_FOO(id)
  WITH_FOO(href)
  WITH_FOO(src)
  WITH_FOO(alt)
  WITH_FOO(type)
  WITH_FOO(onclick)
  WITH_FOO(lang)

  constexpr auto with(const StringImpl &key,
                      const StringImpl &value = "") const {
    auto copy = *this;
    copy.attributes.add(key, value);
    return copy;
  }

  // Helper to add a child tag or string
  template <typename T> constexpr void add_child(const T &child) {
    static_assert(!ContentDisallowed, "Content disallowed for full tags");
    if constexpr (std::is_convertible_v<T, StringImpl>) {
      children.push_back(child);
    } else {
      children.push_back(child.render());
    }
  }

  // Render function
  constexpr StringImpl render(bool with_double_quotes = false) const {
    StringImpl quote = with_double_quotes ? "\"" : "'";
    StringImpl s;
    s += "<";
    s += TagName.value;

    constexpr auto use_default_class = sizeof(ClassNames.value) > 1;
    if constexpr (use_default_class) {
      s += " class=" + quote;
      // todo special case the class name atrribute from arraymainmain
      s += ClassNames.value; // + StringImpl{attributes.class_};
      s += "'";
    }

    for (const auto &[key, value] : attributes.data) {
      if (key.empty()) {
        continue;
      }
      s += " ";
      s += key;

      // allow for empty values, like defer
      // script{foo}.with("defer"), for example
      if (value.empty()) {
        continue;
      }

      s += "=" + quote;
      s += value;
      s += quote;
    }

    if (is_self_closing()) {
      return s + "/>";
    }
    if (is_void()) {
      return s + ">";
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
#define CREATE_TAG(Tag, CloseOption)                                           \
  template <StringLiteral ClassName = "", typename StringImpl = std::string>   \
  struct Tag : tag_base<#Tag, ClassName, CloseOption> {                        \
    using tag_base<#Tag, ClassName, CloseOption>::tag_base;                    \
  };                                                                           \
                                                                               \
  template <StringLiteral ClassName = "", typename StringImpl = std::string>   \
  Tag(const StringImpl &) -> Tag<ClassName>;                                   \
                                                                               \
  template <StringLiteral ClassName = "", typename... T>                       \
  Tag(const T &...) -> Tag<ClassName>;                                         \
                                                                               \
                                                                               \
  template <typename... T> Tag(T &&...) -> Tag<"">;                            \
                                                                               \
  template <StringLiteral ClassName = ""> Tag(const char *) -> Tag<ClassName>; \
                                                                               \
                                                                               \
//   template <hx_pair... Pairs, StringLiteral ClassName = "",                 \
//             typename StringImpl = std::string>                              \
//   Tag(const hx_pair &...) -> Tag<ClassName>;                                \

CREATE_TAG(div, ClosingOption::Full)
CREATE_TAG(p, ClosingOption::Full)
CREATE_TAG(b, ClosingOption::Full)
CREATE_TAG(h1, ClosingOption::Full)
CREATE_TAG(h2, ClosingOption::Full)
CREATE_TAG(h3, ClosingOption::Full)
CREATE_TAG(h4, ClosingOption::Full)
CREATE_TAG(h5, ClosingOption::Full)
CREATE_TAG(h6, ClosingOption::Full)
CREATE_TAG(html, ClosingOption::Full)
CREATE_TAG(span, ClosingOption::Full)
CREATE_TAG(body, ClosingOption::Full)
CREATE_TAG(head, ClosingOption::Full)
CREATE_TAG(title, ClosingOption::Full)
CREATE_TAG(link, ClosingOption::Full)
CREATE_TAG(script, ClosingOption::Full)
CREATE_TAG(style, ClosingOption::Full)
CREATE_TAG(header, ClosingOption::Full)
CREATE_TAG(footer, ClosingOption::Full)
CREATE_TAG(ul, ClosingOption::Full)
CREATE_TAG(ol, ClosingOption::Full)
CREATE_TAG(li, ClosingOption::Full)
CREATE_TAG(nav, ClosingOption::Full)
CREATE_TAG(section, ClosingOption::Full)
CREATE_TAG(article, ClosingOption::Full)
CREATE_TAG(main, ClosingOption::Full)
CREATE_TAG(aside, ClosingOption::Full)
CREATE_TAG(table, ClosingOption::Full)
CREATE_TAG(tr, ClosingOption::Full)
CREATE_TAG(th, ClosingOption::Full)
CREATE_TAG(td, ClosingOption::Full)
CREATE_TAG(thead, ClosingOption::Full)
CREATE_TAG(tbody, ClosingOption::Full)
CREATE_TAG(tfoot, ClosingOption::Full)
CREATE_TAG(col, ClosingOption::Full)
CREATE_TAG(a, ClosingOption::Full)
CREATE_TAG(img, ClosingOption::Full)
CREATE_TAG(input, ClosingOption::Full)
CREATE_TAG(button, ClosingOption::Full)
CREATE_TAG(form, ClosingOption::Full)
CREATE_TAG(svg, ClosingOption::Full)
CREATE_TAG(path, ClosingOption::Full)

CREATE_TAG(meta, ClosingOption::Void)
static_assert(meta{}.is_void());

} // namespace pond
