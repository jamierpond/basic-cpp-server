#include <string>

template<size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N]) {
      std::copy_n(str, N, value); }

    char value[N];
    auto operator<=>(const StringLiteral&) const = default;
    bool operator==(const StringLiteral&) const  = default;
};


template <StringLiteral TagType, StringLiteral ClassName = "", typename... Children>
struct tag_base {
  // tag_base(Children&&... children) : children{std::forward<Children>(children)...} {}

  static constexpr auto tag_name = TagType;
  static constexpr auto class_name = ClassName;

  std::string content = "";
  tag_base(const std::string& str) : content(str) {}
  tag_base(std::initializer_list<tag_base> children) : children{children} {}

  std::tuple<Children...> children;

  std::string render() const {
    using s = std::string;
    auto tag = s{tag_name.value};
    s children_str{};

    std::apply([&children_str](const auto&... child) {
      ((children_str += child.render()), ...);
    }, children);

    return "<" + tag + " class=\"" + class_name.value + "\">" + children_str + "</" + tag + ">";
  }
};

namespace tags {

template<StringLiteral ClassName = "">
struct p : tag_base<"p", ClassName> {};

namespace {
  template<StringLiteral ClassName = "">
  struct div : tag_base<"div", ClassName> {};

  template<StringLiteral ClassName = "">
  div(const std::string& str) -> div<ClassName>;
}

template<StringLiteral ClassName = "">
struct h1 : tag_base<"h1", ClassName> {};
template<StringLiteral ClassName = "">
h1(const std::string& str) -> h1<ClassName>;

template<StringLiteral ClassName = "">
struct nav : tag_base<"nav", ClassName> {};

template<StringLiteral ClassName = "">
struct a : tag_base<"a", ClassName> {};


static_assert(tag_base<"p">::tag_name == "p");
static_assert(tag_base<"a">::tag_name == "a");
static_assert(tag_base<"div">::tag_name == "div");
static_assert(tag_base<"nav">::tag_name == "nav");


}; // namespace tags

