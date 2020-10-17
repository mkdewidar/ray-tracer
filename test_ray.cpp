#include "catch.hpp"

#include "ray.h"
#include <iostream>

TEST_CASE("Ray interpolation") {
    auto r = Ray(Vec3(1, 1, 1), Vec3(1, 2, 3));

    Vec3 result = r.at(2);

    CHECK(result.x == 3);
    CHECK(result.y == 5);
    CHECK(result.z == 7);
}

TEST_CASE("ray_color") {
    auto r = Ray(Vec3(0, 0, 0), Vec3(-1.7778, -1, -1));

    Color actual = ray_color(r);

// -0.78258, -0.44020, -0.44020    -> unit vector
//
// 0.2799         -> lerp factor
//
// 0.7201, 0.7201, 0.7201       -> amount of white
//
// 0.13995, 0.19593, 0.2799      -> amount of blue
//
// 0.86005, 0.91603, 1          -> final color

    auto expected = Color(0.86005, 0.91603, 1.0);

    CHECK(actual.r == Approx(expected.r));
    CHECK(actual.g == Approx(expected.g));
    CHECK(actual.b == Approx(expected.b));
}
