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

constexpr auto NavBar() {
  using namespace pond;
  return pond::nav<"bg-white shadow-md p-4 w-full fixed top-0 left-0 flex justify-between items-center">{
    pond::div<"text-xl font-bold text-gray-900 ml-4">{"Jamie Pond"},
    pond::div<"flex space-x-6 mr-4">{
      a<"text-gray-700 hover:text-blue-500 transition-all">{"LinkedIn"}.with_href(links::LINKEDIN),
      a<"text-gray-700 hover:text-blue-500 transition-all">{"Twitter"}.with_href(links::TWITTER),
      a<"text-gray-700 hover:text-blue-500 transition-all">{"Email"}.with_href(links::EMAIL)
    }
  };
}


template <typename ...T>
constexpr auto layout(std::string title, T&&... body) {
  using namespace pond;
  return
    html {

      head {
        pond::title{title},
        pond::script{}.with_src("https://unpkg.com/@tailwindcss/browser@4")
      },

      pond::body<"text-gray-900 p-4 min-h-screen flex items-center justify-center"> {

      NavBar(),
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

