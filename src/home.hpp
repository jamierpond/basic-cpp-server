#pragma once
#include "tags.hpp"
#include "layout.hpp"
#include "encoding.hpp"
#include "logo_svgs.hpp"

constexpr auto home(const std::string_view& path) {
    using namespace pond;
    // alias p to pond

    return layout (
        "Jamie Pond",
        button {
            p {
                "I'm Lead Audio Software Engineer at ",
                a{"mayk"}.with_href(links::MAYK)
            }.with("hx-get", "/clicked").with("hx-swap", "outerHTML"),
        },
        p {"I build AI-powered music apps with optimized real-time audio engines and fast inference systems."},
        p {
            "Speaker at ", styled::link("CppCon 2024", ""),
            ", ", styled::link("C++ On Sea 2024", ""),
            ", ", styled::link("ADC 2023", ""),
            ", and ", styled::link("ADC 2022", ""), "."
        },
        p {
            "Open-source contributor: ",
            styled::link("thecppzoo/zoo", links::ZOO),
            ", ", styled::link("xtensor-stack/xsimd", links::XSIMD),
            ", ", styled::link("RustAudio/cpal", links::CPAL), "."
        },
        p {
            "I studied ", span<"font-bold">{"Sound & Music Computing MSc"},
            " at ", styled::link("Queen Mary University of London", links::QMUL)
        },
        p { styled::link("Please feel free to hit me up!", links::EMAIL) },
        pond::div<"flex flex-row space-x-4">{
            a{get_linkedin_logo()}.with_href(links::LINKEDIN),
            a{get_bsky_logo()}.with_href(links::BSKY)
        }
    ).render();
}

