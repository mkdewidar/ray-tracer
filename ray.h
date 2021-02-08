#ifndef RAY_H
#define RAY_H

#include "random.h"
#include "vec3.h"
#include "color.h"
#include <cmath>
#include <vector>

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

Color ray_color(Ray const & ray, std::vector<std::unique_ptr<Hittable>> & objects, int depth) {

    // when depth is zero we've bounced off of objects too many times
    // this is a safeguard against blowing the stack
    if (depth <= 0) {
        return Color(0, 0, 0);
    }

    int max_ray_length = 100; // essentially 100 is our view distance

    bool hit_anything = false;

    auto hitResult = HitResult();

    for (std::unique_ptr<Hittable> const & object : objects) {
        // for use inside the loop as an output parameter to the hit function
        auto tmpHitResult = HitResult();

        if (object->hit(ray, 0.00001, max_ray_length, tmpHitResult)) {
            hitResult = tmpHitResult;

            // the t for object becomes our new max length of the ray
            // allowing us to ensure we pick the color of objects that are closest to us
            max_ray_length = hitResult.t;

            hit_anything = true;
        }
    }

    if (hit_anything) {
        // we're imagining that there is a sphere where the normal vector is
        // the radius, then we get a random unit vector from there and follow it
        // to end up in some random place outside the surface we just hit
        auto reflectedRayDirection = (hitResult.point + hitResult.normal + random_unit_vec3()) - hitResult.point;
        auto reflectedRay = Ray(hitResult.point, reflectedRayDirection);

        return 0.5 * ray_color(reflectedRay, objects, depth - 1);
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
