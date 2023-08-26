#ifndef RAY_H
#define RAY_H

#include <cmath>
#include <vector>
#include <limits>

#include "vec3.h"

class Ray {
    public:
        Vec3 orig;
        Vec3 dir;
        double time;

        Ray();

        Ray(Vec3 const & origin, Vec3 const & direction, double time = 0);

        Vec3 at(double const t) const;
};

#include "hittable.h"
#include "material.h"

Color ray_color(Ray const & ray, std::vector<std::unique_ptr<Hittable>> & objects, int depth);

// ------

Ray::Ray() : orig(), dir(), time(0) { }

Ray::Ray(Vec3 const & origin, Vec3 const & direction, double time) : orig(origin), dir(direction), time(time) { }

Vec3 Ray::at(double const t) const {
    return orig + t * dir;
}

// shoot the ray into the world of objects, and find the color that would be seen from the source of the ray
Color ray_color(Ray const & ray, std::vector<std::unique_ptr<Hittable>> & objects, int depth) {

    LOG(
        std::clog << "Bounce number " << depth << "\n";
    )

    // when depth is zero we've bounced off of objects too many times
    // this is a safeguard against blowing the stack
    if (depth <= 0) {
        return Color(0, 0, 0);
    }

    double maxRayLength = std::numeric_limits<double>::infinity(); // essentially our view distance

    bool didHitAnything = false;

    auto hitResult = HitResult();

    // for use inside the loop as an output parameter to the hit function
    auto tmpHitResult = HitResult();

    LOG(
        std::clog << "Ray: " << ray.dir << "\n";
    )

    for (std::unique_ptr<Hittable> const & object : objects) {

        // the 0.00001 is a workaround for fixing "shadow acne"
        // it essentially makes it so that if we collide with something really close, then we ignore it as it might've
        // been a result of a rounding error during the previous collision calculation
        if (object->hit(ray, Interval(0.00001, maxRayLength), tmpHitResult)) {
            hitResult = tmpHitResult;

            // the t for object becomes our new max length of the ray
            // allowing us to ensure we pick the color of objects that are closest to us
            maxRayLength = hitResult.t;

            didHitAnything = true;
            LOG(
                std::clog << "Ray hit object " << object << " with t = " << hitResult.t << ", front face: " << hitResult.isFrontFace << "\n";
                std::clog << "Normal: " << hitResult.normal.x << " " << hitResult.normal.y << " " << hitResult.normal.z << "\n";
            )
        }
    }

    if (didHitAnything) {
        Ray scatteredRay;
        Color attenuation;

        if (hitResult.material->scatter(ray, hitResult, attenuation, scatteredRay))
            return attenuation * ray_color(scatteredRay, objects, depth - 1);
        else {
            return Color(0,0,0);
        }
        // to show a representation of the normals instead of whats above use: 0.5 * (hitResult.normal + Vec3(1, 1, 1))
        // the addition of 1 is to make sure its positive so we don't end up with negative colors
    }

    LOG(
        std::clog << "Hit nothing so falling back to background color" << "\n";
    )

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
