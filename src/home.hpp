#pragma once
#include "tags.hpp"
#include "layout.hpp"
#include "encoding.hpp"
#include "logo_svgs.hpp"

constexpr auto home(const std::string_view& path) {
    using namespace pond;
    // alias p to pond

    return layout (
        "",
        button {
            p {
                "I'm Lead Audio Software Engineer at ",
                styled::link("mayk", links::MAYK),
            }.with("hx-get", "/clicked").with("hx-swap", "outerHTML"),
        },
        p {"I build AI-powered music apps with optimized real-time audio engines and fast inference systems."},
        p {
            "I've spoken at conferences including: ",
            ul <"list-disc list-inside"> {
              li {styled::link("CppCon 2024", links::CPPCON_2024)},
              li {styled::link("C++ On Sea 2024", links::CPPONSEA_2024)},
              li {styled::link("ADC 2023", links::ADC_2023)},
              li {styled::link("ADC 2021", links::ADC_2021)},
            }
        },
        p {
            "Open-source contributor: ",
            styled::link("thecppzoo/zoo", links::ZOO),
            ", ", styled::link("xtensor-stack/xsimd", links::XSIMD),
            ", ", styled::link("RustAudio/cpal", links::CPAL), "."
        },
        p {
            "I studied ", styled::link("Sound & Music Computing MSc", links::QMUL_SMC),
            " at ", styled::link("Queen Mary University of London", links::QMUL)
        },
        p { styled::link("Please feel free to hit me up!", links::EMAIL) },
        pond::div<"flex flex-row space-x-4">{
            a{get_linkedin_logo()}.with_href(links::LINKEDIN),
            a{get_bsky_logo()}.with_href(links::BSKY),
            a{get_github_logo()}.with_href(links::GITHUB)
        }
    ).render();
}

