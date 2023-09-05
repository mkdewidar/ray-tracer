#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include "hittable.h"
#include "ray.h"
#include "vec3.h"
#include "aabb.h"
#include "interval.h"
#include <memory>

// wrap around a Hittable to perform a geometric translation on it by a certain amount
// with this class, you can define Hittables with their dimensions without thinking about their position,
// and even re-use the same Hittable, but apply different transformations for it in the same scene.
// the trick is that it doesn't actually manipulate the points on the object, instead
// it moves the ray itself in the opposite direction, shoots the ray onto just that object
// to calculate whether there is an intersection and where, and then reverses the transformation
// on the intersection point and returns that, so the caller is none the wiser of what happened.
class TranslateTransformer : public Hittable {
    public:
        TranslateTransformer(std::shared_ptr<Hittable> const & target, Vec3 const & offset);

        virtual bool hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const override;

        virtual Aabb bounding_box() const override;

    private:
        std::shared_ptr<Hittable> _target;
        Vec3 _offset;
        Aabb _boundingBox;
};

// ------
TranslateTransformer::TranslateTransformer(std::shared_ptr<Hittable> const & target, Vec3 const & offset)
                                           : _target(target), _offset(offset),
                                             _boundingBox(this->_target->bounding_box() + this->_offset) { }

bool TranslateTransformer::hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const {
    auto transformedRay = Ray(ray.orig - this->_offset, ray.dir, ray.time);

    if (this->_target->hit(transformedRay, rayLimits, result)) {
        result.point = result.point + this->_offset;

        return true;
    } else {
        return false;
    }
}

Aabb TranslateTransformer::bounding_box() const {
    return this->_boundingBox;
}
#endif