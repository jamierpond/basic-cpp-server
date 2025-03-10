#include "layout.hpp"

constexpr auto emily() {
    using namespace html;
    return layout(
            p {
               "Hey Emily, this page is for you!"
            },
            a <"text-blue-500">{
                "Check out Emily's DOPE website!"
            }.with_href("https://emilydzurilla.com/"),
            a <"text-blue-500">{
            "Click here to go back to the home page!"
            }.with_href("/")
    );
}

