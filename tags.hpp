#pragma once

#include <algorithm>
#include <string>
#include <type_traits>
#include <vector>

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

// with_href, with_id, with_src
#define WITH_FOO(Foo)                                                          \
  template <typename T> constexpr auto with_##Foo(const T &t) const {          \
    auto copy = *this;                                                         \
    copy.attributes.Foo = t;                                                   \
    return copy;                                                               \
  }

template <StringLiteral TagName, StringLiteral ClassNames = "",
          typename StringImpl = std::string>
struct tag_base {
private:
  struct Attributes {
    StringImpl style{}, id{}, href{}, src{}, alt{}, type{}, onclick{};
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
  WITH_FOO(style)
  WITH_FOO(id)
  WITH_FOO(href)
  WITH_FOO(src)
  WITH_FOO(alt)
  WITH_FOO(type)
  WITH_FOO(onclick)




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
    if (!attributes.alt.empty()) {
      s += " alt='";
      s += attributes.alt;
      s += "'";
    }
    if (!attributes.type.empty()) {
      s += " type='";
      s += attributes.type;
      s += "'";
    }
    if (!attributes.onclick.empty()) {
      s += " onclick='";
      s += attributes.onclick;
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
