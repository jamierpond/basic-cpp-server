#pragma once

#include <string>
#include <vector>
#include <initializer_list>
#include <sstream>

// String literal template for compile-time string handling
template <size_t N>
struct StringLiteral {
    char value[N];
    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
};

template <StringLiteral TagName, StringLiteral ClassNames = "">
struct tag_base {
private:
    std::string content;
    std::vector<std::string> children;

public:
    // Default constructor for empty tags
    tag_base() = default;

    // Constructor for text content
    tag_base(const std::string& str) : content(str) {}

    // Constructor for text content from string literal
    tag_base(const char* str) : content(str) {}

    // Generic constructor for a single child
    template <typename T>
    tag_base(const T& child) {
        if constexpr (std::is_convertible_v<T, std::string>) {
            content = child;
        } else {
            children.push_back(child.render());
        }
    }

    // Constructor for initializer list (multiple children)
    tag_base(std::initializer_list<std::string> init_children) {
        for (const auto& child : init_children) {
            children.push_back(child);
        }
    }

    // Render the tag with its content and children
    std::string render() const {
        std::stringstream ss;

        ss << "<" << TagName.value;

        // Add class attribute if not empty
        if constexpr (sizeof(ClassNames.value) > 1) {
            ss << " class='" << ClassNames.value << "'";
        }

        ss << ">";

        // Add content if present
        if (!content.empty()) {
            ss << content;
        }

        // Add all children
        for (const auto& child : children) {
            ss << child;
        }

        ss << "</" << TagName.value << ">";

        return ss.str();
    }
};


template <StringLiteral ClassNames = "">
using foo = tag_base<"div", ClassNames>;
