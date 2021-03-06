#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable {

public:
    Point3 center;
    double radius;

    Sphere() { }
    Sphere(Point3 c, double r) : center(c), radius(r) { }

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
    // using the quadratic formula. If you assume a = B.B, b = 2 * B.(A - C), and c = (A - C).(A - C) - r^2
    // then:      at^2 + bt + c
    //
    // the quadtratic formula has the discriminant which allows us to know how many values
    // of t there are for a given instance of the equation. this allows us to tell
    // whether the ray intersects the sphere at multiple points or just one or none.
    //
    // tMin and tMax allow for basic clipping based on the value of t for whatever
    // the ray intersects with.
    virtual bool hit(Ray const & ray, double tMin, double tMax, HitResult & result) const override;

};

bool Sphere::hit(Ray const & ray, double tMin, double tMax, HitResult & result) const {
    Vec3 aMinusC = ray.orig - this->center; // A - C, which is used multiple times below so just do it once

    auto a = ray.dir.length_squared(); // OPTIMISATION: B.B is the same as length of B squared
    // OPTIMISATION: since b = 2 * B.(A - C), the 2 actually gets
    // cancelled out with the 2 in the 2a (denominator) and the 4 in 4ac
    // so instead we're only dealing with half of b instead of b and the rest
    // of the calculations have been adjusted accordingly
    auto halfB = (ray.dir).dot(aMinusC);
    auto c = aMinusC.dot(aMinusC) - (this->radius * this->radius);

    auto discriminant = (halfB * halfB) - (a * c);

    if (discriminant > 0.0) {
        // ray hits sphere in at least one place
        // the rest of the quadratic formula so we can get the value of t
        result.t = (-halfB - sqrt(discriminant)) / a;
        if ((result.t < tMax) && (result.t > tMin)) {
            result.point = ray.at(result.t);
            result.set_face_normal(ray, (result.point - this->center) / this->radius);

            return true;
        }

        // the previous value of t didn't fit in the ray's "length" limits
        // we try the second value
        result.t = (-halfB - sqrt(discriminant)) / a;
        if ((result.t < tMax) && (result.t > tMin)) {
            result.point = ray.at(result.t);
            result.set_face_normal(ray, (result.point - this->center) / this->radius);

            return true;
        }
    }

    return false;
}

#endif
