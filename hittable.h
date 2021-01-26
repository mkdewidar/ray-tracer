#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

// contains the result of a intersection
struct HitResult {
    Point3 point;
    Vec3 normal;
    bool front_face;
    double t = -1.0;

    // sets the normal field as well as the face based on the direction of the
    // normal.
    // if the normal is in the opposite direction of the ray then the normal
    // is pointing outwards, therefore we hit the front of the face.
    inline void set_face_normal(Ray const & ray, Vec3 const & normal) {
        front_face = ray.dir.dot(normal) < 0;
        this->normal = front_face ? normal : -normal;
    }
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
