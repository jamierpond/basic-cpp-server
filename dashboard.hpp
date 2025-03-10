#pragma once

#include "layout.hpp"
#include "tags.hpp"
#include <string>

namespace dashboard {
constexpr auto generate_card(const std::string &title,
                             const std::string &content) {
  using namespace pond;
  return pond::div<"bg-white shadow-lg rounded-2xl p-6 max-w-sm w-full">{
      h3<"text-xl font-bold text-gray-900">{title},
      p<"text-gray-600 mt-2">{content}};
}

constexpr auto generate_stat_card(const std::string &label, int value) {
  using namespace pond;
  return pond::div<"bg-gradient-to-r from-blue-500 to-indigo-500 p-4 "
                   "rounded-xl shadow-md text-white text-center">{
      h3<"text-lg font-semibold">{label},
      p<"text-2xl font-bold">{std::to_string(value)}};
}

constexpr auto generate_sales_chart() {
  using namespace pond;
  return pond::div<"bg-white shadow-lg rounded-2xl p-6 w-full h-64 flex "
                   "flex-col justify-center">{
      h3<"text-lg font-semibold text-center text-gray-900 mb-4">{
          "Sales Over Time"},
      pond::div<"w-full h-48 bg-gray-200 rounded-md flex items-end">{
          pond::div<"bg-blue-500 w-1/6 h-1/6 mx-1">{},
          pond::div<"bg-blue-500 w-1/6 h-1/3 mx-1">{},
          pond::div<"bg-blue-500 w-1/6 h-1/2 mx-1">{},
          pond::div<"bg-blue-500 w-1/6 h-2/3 mx-1">{},
          pond::div<"bg-blue-500 w-1/6 h-3/4 mx-1">{},
          pond::div<"bg-blue-500 w-1/6 h-full mx-1">{}}};
}

constexpr auto dashboard() {
  using namespace pond;
  return layout(
            "Dashboard",
             h1<"text-3xl font-bold text-center text-gray-900 my-6">{
                 "Dashboard Overview"},
             pond::div<"grid grid-cols-3 gap-6">{
                 generate_stat_card("Users", 1289),
                 generate_stat_card("Sales", 764),
                 generate_stat_card("Revenue ($)", 45320)},
             pond::div<"grid grid-cols-2 gap-6 mt-6">{
                 generate_sales_chart(),
                 generate_card("Recent Activity",
                               "User JohnDoe purchased a premium plan."),
                 generate_card("System Status",
                               "All services are operational.")})
      .render();
}

} // namespace dashboard
