#ifndef RAY_H
#define RAY_H

#include "vec3.h"

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

#endif
