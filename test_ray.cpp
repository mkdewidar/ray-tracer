#include "catch.hpp"

#include "ray.h"

TEST_CASE("Ray interpolation") {
    auto r = Ray(Vec3(1, 1, 1), Vec3(1, 2, 3));

    Vec3 result = r.at(2);

    REQUIRE(result.x == 3);
    REQUIRE(result.y == 5);
    REQUIRE(result.z == 7);
}
