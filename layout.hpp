#pragma once
#include "tags.hpp"

namespace links {
  constexpr auto LINKEDIN = "https://www.linkedin.com/in/jamierpond";
  constexpr auto TWITTER = "https://twitter.com/jamiepondx";
  constexpr auto EMAIL = "mailto:jamie@pond.audio";

  constexpr auto MAYK = "https://www.mayk.it";
  constexpr auto QMUL = "https://www.qmul.ac.uk";
  constexpr auto ADC_2021 = "https://www.youtube.com/watch?v=X8dPANPmC7E&ab_channel=JUCE";
  constexpr auto ADC_2023 = "https://www.youtube.com/watch?v=1lEWl-MTA6k&ab_channel=ADC-AudioDeveloperConference";

  constexpr auto LINKEDIN_IMG = "https://content.linkedin.com/content/dam/me/business/en-us/amp/brand-site/v2/bg/LI-Bug.svg.original.svg";
  constexpr auto TWITTER_IMG = "https://seeklogo.com/images/T/twitter-x-logo-101C7D2420-seeklogo.com.png?v=638258862800000000";
};

template <typename ...T>
constexpr auto layout(T&&... body) {
    using namespace html;
    return html::div<"w-full flex flex-col items-center">{{
        a<"text-4xl font-bold text-center">{"Jamie Pond"}.with_href(links::LINKEDIN),
        h3<"text-2xl font-bold text-center mt-4">{a{"jamie@pond.audio"}.with_href(links::EMAIL)},
        html::div<"flex flex-col text-center items-center justify-center p-8 space-y-8">{{
          {std::forward<T>(body)...}
        }}
    }};
}

