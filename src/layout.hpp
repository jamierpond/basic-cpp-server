#pragma once
#include "tags.hpp"

namespace links {
  constexpr auto CPAL = "https://github.com/rustaudio/cpal";
  constexpr auto ZOO = "https://github.com/thecppzoo/zoo";
  constexpr auto XSIMD = "https://github.com/xtensor-stack/xsimd";

  constexpr auto GITHUB = "https://github.com/jamierpond";
  constexpr auto LINKEDIN = "https://www.linkedin.com/in/jamierpond";
  constexpr auto BSKY = "https://bsky.app/profile/pond.audio";
  constexpr auto EMAIL = "mailto:jamie@pond.audio";

  constexpr auto MAYK = "https://www.mayk.it";
  constexpr auto QMUL = "https://www.qmul.ac.uk";
  constexpr auto ADC_2021 = "https://www.youtube.com/watch?v=X8dPANPmC7E&ab_channel=JUCE";
  constexpr auto ADC_2023 = "https://www.youtube.com/watch?v=1lEWl-MTA6k&ab_channel=ADC-AudioDeveloperConference";

  constexpr auto LINKEDIN_IMG = "https://content.linkedin.com/content/dam/me/business/en-us/amp/brand-site/v2/bg/LI-Bug.svg.original.svg";
};

constexpr auto nav_bar() {
  using namespace pond;
  return pond::nav<"bg-white shadow-md p-4 w-full fixed top-0 left-0 flex justify-between items-center">{
    pond::div<"text-xl font-bold text-gray-900 ml-4">{"Jamie Pond"},
    pond::div<"flex space-x-6 mr-4">{
      a<"text-gray-700 hover:text-blue-500 transition-all">{"GitHub"}.with_href(links::GITHUB),
      a<"text-gray-700 hover:text-blue-500 transition-all">{"LinkedIn"}.with_href(links::LINKEDIN),
      a<"text-gray-700 hover:text-blue-500 transition-all">{"BSky"}.with_href(links::BSKY),
      a<"text-gray-700 hover:text-blue-500 transition-all">{"Email"}.with_href(links::EMAIL),
    }
  };
}


template <typename ...T>
constexpr auto layout(std::string title, T&&... body) {
  using namespace pond;
  return
    html {
      head {
          pond::meta{}.with("charset", "UTF-8"),
          pond::meta{}.with("name", "viewport")
                      .with("content", "width=device-width, initial-scale=1.0"),
          pond::script{}.with("src", "https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4").with("defer", "false"),
          pond::script{}.with("src", "https://unpkg.com/htmx.org@2.0.4")
            .with("defer", "true"),
          pond::title{"Jamie Pond's C++ HTTP Server"}
      },
      pond::body<"text-gray-900 p-4 min-h-screen flex items-center justify-center"> {
          nav_bar(),
          pond::div <"min-h-screen flex items-center justify-center"> {
            pond::div<"w-full flex flex-col items-center">{
              pond::div<"flex flex-col text-center items-center justify-center p-8 space-y-8">{
                std::forward<T>(body)...
              }
            }
          }
      }
    };
}

namespace styled {
  constexpr auto link = [](auto& text, auto& path) {
      return pond::a<"text-blue-500">{text}.with_href(path);
  };
};


