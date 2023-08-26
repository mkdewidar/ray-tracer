#ifndef HITTABLE_H
#define HITTABLE_H

#include "vec3.h"
#include "ray.h"

class Material;

// contains the result of a ray intersecting with an object
class HitResult {
    public:
        Point3 point;
        Vec3 normal;
        // whether we hit the front of the face or the back
        bool isFrontFace;
        std::shared_ptr<Material> material;
        // the scalar that if you multiply by the ray takes you to the point at which the intersection occurred
        double t = -1.0;

        void set_face_normal(Ray const & ray, Vec3 const & normal);
};

class Hittable {
    public:
        virtual ~Hittable() = default;

        // override this to define how to detect a intersection with the given ray.
        // returns true if ray interests or false otherwise, intersection properties
        // are returned in the "result" parameter
        // min and max are to control the "length" of the ray
        virtual bool hit(Ray const & ray, double tMin, double tMax, HitResult & result) const = 0;
};

// ------

// sets the normal field as well as the face based on the direction of the
// normal.
// if the normal is in the opposite direction of the ray then the normal
// is pointing outwards, therefore we hit the front of the face.
void HitResult::set_face_normal(Ray const & ray, Vec3 const & normal) {
    isFrontFace = ray.dir.dot(normal) < 0;
    this->normal = isFrontFace ? normal : -normal;
}

#endif
