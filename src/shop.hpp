#pragma once
#include "layout.hpp"
#include "tags.hpp"

constexpr auto cart_script = R"(
document.addEventListener("DOMContentLoaded", () => {
    let cart = JSON.parse(localStorage.getItem("cart")) || [];
    updateCartDisplay(cart);
});

function addToCart(item) {
    let cart = JSON.parse(localStorage.getItem("cart")) || [];
    cart.push(item);
    localStorage.setItem("cart", JSON.stringify(cart));
    updateCartDisplay(cart);
}

function updateCartDisplay(cart) {
    let cartList = document.getElementById("cart-list");
    cartList.innerHTML = "";
    cart.forEach(item => {
        let li = document.createElement("li");
        li.textContent = item;
        cartList.appendChild(li);
    });
}
)";

constexpr auto shop() {
    using namespace pond;
    return layout(
        "Cart",
        script{cart_script},
        h1<"text-3xl font-bold text-center text-gray-900 my-6">{"Shopping Cart"},
        pond::div<"p-4">{
            p{"Click an item to add it to the cart:"},
            pond::div<"flex space-x-4">{
                a<"bg-blue-500 text-white p-2 rounded-md">{"Add Apple"}
                    .with_onclick("addToCart(\"Apple\")"),
                a{"Add Banana"}
                    .with_onclick("addToCart(\"Banana\")"),
                button<"bg-red-500 text-white p-2 rounded-md">{"Add Orange"}
                    .with_onclick("console.log(\"Orange\")")
            },
            h2<"text-xl font-semibold mt-4">{"Cart Items:"},
            ul<"list-disc pl-6">().with_id("cart-list")
        }
    ).render();
}

