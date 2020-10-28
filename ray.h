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

// a sphere is described using the equation x^2 + y^2 + z^2 = r^2
// meanwhile, r can also be described as the magnitidue of the vector P - C
// where C is the center of the sphere and P is a point on the sphere
// additionally, a vector that is dot producted with itself produces the length squared
// with all that in mind, (P - C).(P - C) = r^2
// relative to our Rays, P = A + tB where A is the origin of the ray, t is some factor
// and B is the direction.
//
// we can expand the equation as follows:
//    (P - C).(P - C) = r^2
//    (A + tB - C).(A + tB - C) = r^2                              substituted P = A + tB
//    A.(A + tB - C) + tB.(A + tB - C) - C.(A + tB - C) = r^2      expanded (aka distribute)
//    A.A + A.tB + A.-C + tB.A + tB.tB + tB.-C + -C.A + -C.tB + -C.-C = r^2
//    A.A + 2(A.tB) + 2(A.-C) + tB.tB + 2(tB.-C) + -C.-C = r^2     combined similar elements
//
//    tB.tB becomes t^2(B.B) (due to associative law for scalar and dot product)
//    2(A.tB) and 2(tB.-C) becomes 2tB.(A - C) (due to dot product distributivity)
//    A.A + 2(A.-C) + -C.-C becomes (A - C).(A - C) (due to matching a quadratic formula)
//
// therefore:    t^2(B.B) + 2tB.(A - C) + (A - C).(A - C) - r^2 = 0
//
// since we know everything except t, they can all be considered constants, leaving t
// as the only unknown, making this function a quadtratic equation that is solvable
// using the quadratic formula. If you assume a = B.B, b = (A - C), and c = (A - C).(A - C) - r^2
// then:      at^2 + bt + c
//
// the quadtratic formula has the discriminant which allows us to know how many values
// of t there are for a given instance of the equation. this allows us to tell
// whether the ray intersects the sphere at multiple points or just one or none.
double hit_sphere(Vec3 const & sphere, double radius, Ray const & ray) {
    auto aMinusC = ray.orig - sphere; // A - C, which is used multiple times below so just do it once

    auto a = ray.dir.length_squared(); // B.B is the same as length of B squared
    auto b = (2.0 * ray.dir).dot(aMinusC);
    auto c = aMinusC.dot(aMinusC) - (radius * radius);

    auto discriminant = (b * b) - (4 * a * c);

    if (discriminant < 0.0) { // ray does not hit sphere
        return -1;
    } else { // ray hits sphere in at least one place
        // the rest of the quadratic formula so we can get the value of t
        return (-b - sqrt(discriminant)) / (2 * a);
    }
}

Color ray_color(Ray const & ray) {
    auto sphereCenter = Vec3(0, 0, -1.0);
    double t = hit_sphere(sphereCenter, 0.5, ray);
    if (t > 0.0) {
        Vec3 surfaceNormal = (ray.at(t) - sphereCenter).unit();
        // convert the normal's axis from range of -1, 1 to 0, 1 and use that as the color
        return 0.5 * Color(surfaceNormal.x + 1, surfaceNormal.y + 1, surfaceNormal.z + 1);
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
