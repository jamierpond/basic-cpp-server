#pragma once
#include "tags.hpp"
#include "layout.hpp"

static_assert(html::p <"text-blue-500">{"p"}.render() == "<p class='text-blue-500'>p</p>");


constexpr auto home() {
    using namespace html;
    return layout(
            h3<"text-2xl font-bold text-center mt-4">{a{"jamie@pond.audio"}.with_href(links::EMAIL)},
            p {{
               "I'm Lead Audio Software Engineer at ", a{"mayk"}
               .with_href(links::MAYK)
            }},
            p {{
              "I studied ", span<"font-bold">{"Sound & Music Computing MSc"},
              " at ", a<"text-blue-500"> {"Queen Mary University of London"}
              .with_href(links::QMUL)
            }},
            p {{
              "I spoke at ADC 2021 about ", a{span<"font-bold text-blue-500">{"using compiler intrinsics in your code"}}
              .with_href(links::ADC_2021)
            }},
            a <"text-blue-500"> {
              "Click here to go to emily's page"
            }.with_href("/emily"),
            p {{
              "I spoke at ADC 2023 about ", a{span<"font-bold">{"mayk's approach to prototyping"}}
              .with_href(links::ADC_2023)
            }},
            p {"I'm a mentor in the ADC 2023 & 2024 Mentorship Program."},
            p {
              a<"underline">{"Please feel free to hit me up!"}
              .with_href(links::EMAIL)
            },
            html::div<"flex flex-row space-x-4">{{
                a{img<"w-8 h-8">{}.with_src(links::LINKEDIN_IMG)}.with_href(links::LINKEDIN),
                a{img<"w-8 h-8">{}.with_src(links::TWITTER_IMG)}.with_href(links::TWITTER)
            }}
    );
}

static_assert(layout(html::p{"foo"}).render().length() > 0);

