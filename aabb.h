#ifndef AABB_H
#define AABB_H

#include <cmath>

#include "interval.h"

// an implementation of axis-aligned bounding boxes to be used by the ray tracer's BVH
// this AABB is defined by 3 intervals along the three axis, figuring out whether a ray intersects
// with it is therefore as simple as checking that the ray's various components (x, y, z) intersect with
// all three of the intervals.
class Aabb {
    public:
        Interval xBounds, yBounds, zBounds;

        Aabb();
        Aabb(Interval const & xInterval, Interval const & yInterval, Interval const & zInterval);
        // for creating AABB that encloses these two points
        Aabb(Point3 const & a, Point3 const & b);
        // for creating AABB that encloses these two other AABB entirely
        Aabb(Aabb const & a, Aabb const & b);

        bool hit(Ray const & incomingRay, Interval rayLimits) const;
    private:
        bool intersect_with_bounds(Interval const & componentBounds, double const rayDirectionComponent,
                                   double const rayOriginComponent, Interval & rayLimits) const;
};

Aabb::Aabb() : xBounds(), yBounds(), zBounds() { }

Aabb::Aabb(Interval const & xInterval, Interval const & yInterval, Interval const & zInterval)
           : xBounds(xInterval), yBounds(yInterval), zBounds(zInterval) { }

Aabb::Aabb(Point3 const & a, Point3 const & b) {
    this->xBounds = Interval(fmin(a.x, b.x), fmax(a.x, b.x));
    this->yBounds = Interval(fmin(a.y, b.y), fmax(a.y, b.y));
    this->zBounds = Interval(fmin(a.z, b.z), fmax(a.z, b.z));
}

Aabb::Aabb(Aabb const & a, Aabb const & b) {
    this->xBounds = Interval(a.xBounds, b.xBounds);
    this->yBounds = Interval(a.yBounds, b.yBounds);
    this->zBounds = Interval(a.zBounds, b.zBounds);
}

bool Aabb::hit(Ray const & incomingRay, Interval rayLimits) const {
    // by re-ordering the equation for ray intersecting with a point, we can find t (the scalar at which the ray
    // intersects with a point) using (P(t) - A) / b
    // using that equation for each axis individually, we can identify if the ray intersects with any axis
    // note that we also need to consider the following possibilities:
    //     the ray is forward and origin is before the bound: all t will be positive
    //     the ray is forward and origin is inside the bound: t0 will be negative and t1 will be positive
    //     the ray is forward and origin is after the bound:  all t will be negative
    //     the ray is backward and origin is before the bound: all t will be negative
    //     the ray is backward and origin is inside the bound: t0 will be positive and t1 will be negative
    //     the ray is backward and origin is after the bound:  all t will be positive
    //     the ray is parallel to the bound: there is no t
    // NOTE: rayLimits is modified by all these functions to include the new closest and furthest intersection
    //     t values of the ray
    return intersect_with_bounds(xBounds, incomingRay.dir.x, incomingRay.orig.x, rayLimits) &&
           intersect_with_bounds(yBounds, incomingRay.dir.y, incomingRay.orig.y, rayLimits) &&
           intersect_with_bounds(zBounds, incomingRay.dir.z, incomingRay.orig.z, rayLimits);
}

bool Aabb::intersect_with_bounds(Interval const & componentBounds, double const rayDirectionComponent,
                                 double const rayOriginComponent, Interval & rayLimits) const {
    auto invD = 1 / rayDirectionComponent;

    // the t for the intersection with the lower bound
    auto t0 = (componentBounds.min - rayOriginComponent) * invD;

    // the t for the intersection with the upper bound
    auto t1 = (componentBounds.max - rayOriginComponent) * invD;

    if (invD < 0)
        std::swap(t0, t1);

    if (t0 > rayLimits.min)
        rayLimits.min = t0;
    if (t1 < rayLimits.max)
        rayLimits.max = t1;

    if (rayLimits.max <= rayLimits.min)
        return false;

    return true;
}

#endif
