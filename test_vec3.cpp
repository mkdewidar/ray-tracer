#include "catch.hpp"

#include "vec3.h"
#include <cmath>
#include <iostream>

TEST_CASE("Vec3.length") {
    auto v = Vec3(1.75, -2.5, 4);

    CHECK(v.length() == Approx(5.03115));
}

TEST_CASE("Vec3 * constant") {
    auto v = Vec3(1.75, -2.5, 4);

    auto expected = Vec3(3.5, -5, 8);

    auto actual = v * 2;

    CHECK(actual.x == expected.x);
    CHECK(actual.y == expected.y);
    CHECK(actual.z == expected.z);
}

TEST_CASE("Vec3.unit") {
    auto v = Vec3(1.75, -2.5, 4);

    auto actual = v.unit();

    CHECK(actual.x == (v.x / v.length()));
    CHECK(actual.y == (v.y / v.length()));
    CHECK(actual.z == (v.z / v.length()));
}
