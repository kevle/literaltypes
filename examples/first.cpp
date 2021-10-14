#include "literaltypes/ConstString.hpp"
#include "literaltypes/Values.hpp"

#include <iostream>

using literaltypes::operator""_c;

int main()
{
    auto c = "float width;"
             "float length;"
             "double height;"
             "int num_windows;"_c;

    // struct Container
    // {
    //     float width;
    //     float length;
    //     double height;
    //     int num_windows;
    // };

    // Access members
    c.get<"width">() = 2.f;
    c.get<"length">() = 3.f;

    // Const access to members
    std::cout << "width: " << std::as_const(c).get<"width">() << '\n';

    // It is default initialized
    std::cout << "num_windows: " << c.get<"num_windows">() << '\n';

    // Iterate over all members
    c.apply_all(
        [](const auto& v, std::string_view name) { std::cout << name << ": " << v << '\n'; });
}
