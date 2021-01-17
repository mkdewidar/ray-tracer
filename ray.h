#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include "color.h"
#include <cmath>

class Ray {
    public:
        Vec3 orig;
        Vec3 dir;

        Ray() { }

        Ray(Vec3 const & origin, Vec3 const & direction)
            : orig(origin), dir(direction) { }

        Vec3 at(double const t) const {
            return orig + t * dir;
        }
};

#include "sphere.h"

Color ray_color(Ray const & ray) {
    auto sphere = Sphere(Vec3(0, 0, -1.0), 0.5);

    auto hitResult = HitResult();
    if (sphere.hit(ray, 0.5, 100, hitResult)) {
        // convert the normal's axis from range of -1, 1 to 0, 1 and use that as the color
        return 0.5 * Color(hitResult.normal.x + 1, hitResult.normal.y + 1, hitResult.normal.z + 1);
    }

    Vec3 unitDirection = ray.dir.unit();
    // by default any component of a unit vector is in the range of -1 -> 1
    // this changes the y so that it is from 0 -> 1 (i.e linear interpolation)
    auto lerpFactor = 0.5 * (unitDirection.y + 1.0);
    // use the factor we calculated above to decide how much blue and how much
    // white should be in the color returned. 1 means blue while 0 means white.
    return ((1.0 - lerpFactor) * Color(1.0, 1.0, 1.0)) +
        (lerpFactor * Color(0.5, 0.7, 1.0));
}

#endif
