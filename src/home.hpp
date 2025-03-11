#pragma once
#include "tags.hpp"
#include "layout.hpp"
#include "encoding.hpp"
#include <cstdint>
#include <iostream>

struct CounterState {
    uint64_t count = 0;
};

using Encoding = StringEncoder<char_sets::Base62Encoding, uint64_t, true, true>;

constexpr auto scroll_position_script = R"(
document.addEventListener("DOMContentLoaded", function(event) {
    var scrollpos = localStorage.getItem('scrollpos');
    if (scrollpos) window.scrollTo(0, scrollpos);
});

window.onbeforeunload = function(e) {
    localStorage.setItem('scrollpos', window.scrollY);
};
)";


constexpr auto home(const std::string_view& path) {
    // trim the leading slash
    auto count_str = path.substr(1);

    auto count = uint64_t{0};
    try {
      count = Encoding::decode(count_str);
    } catch (const std::runtime_error &e) {
      std::cerr << "Invalid count: " << count_str << "\n";
      count = 0;
    }

    auto encode = [] (const CounterState &state) {
        return Encoding::encode(state.count);
    };

    using namespace pond;
    return layout(
            "Jamie Pond",
            script {scroll_position_script},
            p {
               "I'm Lead Audio Software Engineer at ",
               a{"mayk"}.with_href(links::MAYK)
            },
            p {
              "I studied ", span<"font-bold">{"Sound & Music Computing MSc"},
              " at ", a<"text-blue-500">{"Queen Mary University of London"}
              .with_href(links::QMUL)
            },
            p {
              "I spoke at ADC 2021 about ", a{span<"font-bold text-blue-500">{"using compiler intrinsics in your code"}}
              .with_href(links::ADC_2021)
            },
            a <"bg-blue-500 text-white p-2 rounded-md">{
              "Increment the counter!"
            }.with_href("/" + encode({.count = count + 1})),
            a <"bg-blue-500 text-white p-2 rounded-md">{
              "Decrement the counter!"
            }.with_href("/" + encode({.count = count - 1})),
            a <"bg-blue-500 text-white p-2 rounded-md">{
              "Reset the counter!"
            }.with_href("/" + encode({.count = 0})),
            p {
              "You have clicked the button ", span<"font-bold">{std::to_string(count)},
              " times."
            },
            a <"text-blue-500"> {
              "Click here to go to emily's page"
            }.with_href("/emily"),
            p {
              "I spoke at ADC 2023 about ", a{span<"font-bold">{"mayk's approach to prototyping"}}
              .with_href(links::ADC_2023)
            },
            p {"I'm a mentor in the ADC 2023 & 2024 Mentorship Program."},
            p {
              a<"text-blue-500 underline">{"Please feel free to hit me up!"}
              .with_href(links::EMAIL)
            },
            pond::div<"flex flex-row space-x-4">{
                a{img<"w-8 h-8">{}.with_src(links::LINKEDIN_IMG)}.with_href(links::LINKEDIN),
                a{img<"w-8 h-8">{}.with_src(links::TWITTER_IMG)}.with_href(links::TWITTER)
            }
    ).render();
}

static_assert(layout("test", pond::p{"foo"}).render().length() > 0);
