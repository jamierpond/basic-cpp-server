#include "layout.hpp"

constexpr auto increment_script = R"(
document.addEventListener("DOMContentLoaded", () => {
let count = localStorage.getItem("count") || 0;
document.getElementById("counter").innerText = count;
});

function increment() {
    let count = parseInt(localStorage.getItem("count") || "0") + 1;
    localStorage.setItem("count", count);
    document.getElementById("counter").innerText = count;
}
)";

constexpr auto emily() {
    using namespace pond;
    return layout(
            "Emily Dzurilla",
            p {
               "Hey Emily, this page is for you!"
            },
            a <"text-blue-500">{
                "Check out Emily's DOPE website!"
            }.with_href("https://emilydzurilla.com/"),
            a <"text-blue-500">{
            "Click here to go back to the home page!"
            }.with_href("/"),
            script {increment_script},
            // display the counter
            p { "You have clicked the button ", span<"font-bold">{"0"}
                .with_id("counter"), " times." },
            button<"bg-blue-500 text-white p-2 rounded-md">{"Increment"}
                .with_onclick("increment()")
    ).render();
}

