#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

// contains the result of a intersection
struct HitResult {
    Point3 point;
    Vec3 normal;
    double t = -1.0;
};

class Hittable {

public:
    // override this to define how to detect a intersection with the given ray.
    // returns true if ray interesects or false otherwise, intersection properties
    // are returned in the "result" parameter
    // min and max are to control the "length" of the ray
    virtual bool hit(Ray const & ray, double tMin, double tMax, HitResult & result) const = 0;
};

#endif
