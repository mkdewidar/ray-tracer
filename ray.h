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

Color ray_color(Ray const & ray, std::shared_ptr<Hittable> const & world, int depth, Color const & backgroundColor);

// ------

Ray::Ray() : orig(), dir(), time(0) { }

Ray::Ray(Vec3 const & origin, Vec3 const & direction, double time) : orig(origin), dir(direction), time(time) { }

Vec3 Ray::at(double const t) const {
    return orig + t * dir;
}

// shoot the ray into the world of objects, and find the color emitted at the end of that ray's journey,
// while keeping track of any materials you hit on the way whose attenuation affects what color is seen in the pixel
Color ray_color(Ray const & ray, std::shared_ptr<Hittable> const & world, int depth, Color const & backgroundColor) {

    LOG(
        std::clog << "Bounce number " << depth << "\n";
    )

    // when depth is zero we've bounced off of objects too many times
    // this is a safeguard against blowing the stack
    if (depth <= 0) {
        return Color(0, 0, 0);
    }

    double maxRayLength = std::numeric_limits<double>::infinity(); // essentially our view distance

    auto hitResult = HitResult();

    LOG(
        std::clog << "Ray: " << ray.dir << "\n";
    )

    // the 0.00001 is a workaround for fixing "shadow acne"
    // it essentially makes it so that if we collide with something really close, then we ignore it as it might've
    // been a result of a rounding error during the previous collision calculation
    if (world->hit(ray, Interval(0.00001, maxRayLength), hitResult)) {
        Ray scatteredRay;
        Color attenuation;

        // the color emitted by the object we hit
        Color emittedColor = hitResult.material->emitted(hitResult.u, hitResult.v, hitResult.point);

        // if this object's material bounces rays, then find out what color results from the bounce
        // by following the bounce to the original light source, attentuation is how much that original
        // light source's color was affected by this material
        if (!hitResult.material->scatter(ray, hitResult, attenuation, scatteredRay)) {
            // this material doesn't reflect, so the color we see is whatever light it emits
            return emittedColor;
        }

        Color attenuatedColor = attenuation * ray_color(scatteredRay, world, depth - 1, backgroundColor);

        // how come in the book they add the emitted color to this, but in practice it doesn't seem to make a difference
        return attenuatedColor;
        // to show a representation of the normals instead of whats above use: 0.5 * (hitResult.normal + Vec3(1, 1, 1))
        // the addition of 1 is to make sure its positive so we don't end up with negative colors
    }

    LOG(
        std::clog << "Hit nothing so falling back to background color" << "\n";
    )

    return backgroundColor;

//    Vec3 unitDirection = ray.dir.unit();
//    // by default any component of a unit vector is in the range of -1 -> 1
//    // this changes the y so that it is from 0 -> 1 (i.e linear interpolation)
//    auto lerpFactor = 0.5 * (unitDirection.y + 1.0);
//    // use the factor we calculated above to decide how much blue and how much
//    // white should be in the color returned. 1 means blue while 0 means white.
//    return ((1.0 - lerpFactor) * Color(1.0, 1.0, 1.0)) +
//        (lerpFactor * Color(0.5, 0.7, 1.0));
}

#endif
