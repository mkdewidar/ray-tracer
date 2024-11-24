
#include "catch.hpp"
#include "image.h"

TEST_CASE("Load image and read pixel") {
    auto i = Image("./earthmap.jpg");

    CHECK(i.width == 1024);
    CHECK(i.height == 512);
    
    double * const p = i.color_at(100, 100);
    CHECK(p != nullptr);
    std::cerr << "The value at pixel 100, 100 is: " << p[0] << ", " << p[1] << ", " << p[2] << "\n";
}