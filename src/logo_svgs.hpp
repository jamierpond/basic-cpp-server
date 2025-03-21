#include <string>
#include "tags.hpp"

constexpr auto BSKY_LOGO_SVG_PATH = "m135.72 44.03c66.496 49.921 138.02 151.14 164.28 205.46 26.262-54.316 97.782-155.54 164.28-205.46 47.98-36.021 125.72-63.892 125.72 24.795 0 17.712-10.155 148.79-16.111 170.07-20.703 73.984-96.144 92.854-163.25 81.433 117.3 19.964 147.14 86.092 82.697 152.22-122.39 125.59-175.91-31.511-189.63-71.766-2.514-7.3797-3.6904-10.832-3.7077-7.8964-0.0174-2.9357-1.1937 0.51669-3.7077 7.8964-13.714 40.255-67.233 197.36-189.63 71.766-64.444-66.128-34.605-132.26 82.697-152.22-67.108 11.421-142.55-7.4491-163.25-81.433-5.9562-21.282-16.111-152.36-16.111-170.07 0-88.687 77.742-60.816 125.72-24.795z";

template <typename ...T>
constexpr auto create_homepage_svg (std::string og_w, std::string og_h, T... paths) {
    auto svg = pond::svg<"w-full h-full"> {
      std::forward<T>(paths)...
    }
    .with("version", "1.1")
    .with("xmlns", "http://www.w3.org/2000/svg")
    .with("width", og_w)
    .with("height", og_h)
    .with("viewBox", "0 0 " + og_w + " " + og_h);

  return pond::div<"w-8 h-8"> {
    svg
  };
};

constexpr auto get_bsky_logo() {
  return create_homepage_svg("600", "530",
    pond::path{}.with("d", BSKY_LOGO_SVG_PATH)
      .with("fill", "#1185fe")
  );
}

constexpr auto li_one = "M1165,274.515a1.2,1.2,0,0,0,1.21-1.269c0-.9-.543-1.33-1.657-1.33h-1.8v4.712h.677v-2.054h.832l.019.025,1.291,2.029h.724l-1.389-2.1Zm-.783-.472h-.785V272.45h.995c.514,0,1.1.084,1.1.757,0,.774-.593.836-1.314.836' transform='translate(-1092.136 -213.406)";
constexpr auto li_two = "M958.98,112.559h-9.6V97.525c0-3.585-.064-8.2-4.993-8.2-5,0-5.765,3.906-5.765,7.939v15.294h-9.6V81.642h9.216v4.225h.129a10.1,10.1,0,0,1,9.093-4.994c9.73,0,11.524,6.4,11.524,14.726ZM918.19,77.416a5.571,5.571,0,1,1,5.57-5.572,5.571,5.571,0,0,1-5.57,5.572m4.8,35.143h-9.61V81.642h9.61Zm40.776-55.2h-55.21a4.728,4.728,0,0,0-4.781,4.67v55.439a4.731,4.731,0,0,0,4.781,4.675h55.21a4.741,4.741,0,0,0,4.8-4.675V62.025a4.738,4.738,0,0,0-4.8-4.67' transform='translate(-903.776 -57.355)";
constexpr auto li_the = "M1156.525,264.22a4.418,4.418,0,1,0,.085,0h-.085m0,8.33a3.874,3.874,0,1,1,3.809-3.938c0,.022,0,.043,0,.065a3.791,3.791,0,0,1-3.708,3.871h-.1' transform='translate(-1084.362 -207.809)";
constexpr auto get_linkedin_logo() {
  constexpr auto li_colour = "#0a66c2";
  return create_homepage_svg("76.624", "65.326",
    pond::path{}.with("d", li_one).with("fill", li_colour),
    pond::path{}.with("d", li_two).with("fill", li_colour),
    pond::path{}.with("d", li_the).with("fill", li_colour)
  );
}

