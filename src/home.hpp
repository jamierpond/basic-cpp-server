#pragma once
#include "tags.hpp"
#include "layout.hpp"
#include "encoding.hpp"
#include "logo_svgs.hpp"

constexpr auto home(const std::string_view& path) {
    using namespace pond;
    return layout (
            "Jamie Pond",
            button {
              p {
                 "I'm Lead Audio Software Engineer at ",
                 a{"mayk"}.with_href(links::MAYK)
              }.with("hx-get", "/clicked").with("hx-swap", "outerHTML"),
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
                a{get_linkedin_logo()}.with_href(links::LINKEDIN),
                a{get_bsky_logo()}.with_href(links::BSKY)
            }
    ).render();
}

static_assert(layout("test", pond::p{"foo"}).render().length() > 0);
