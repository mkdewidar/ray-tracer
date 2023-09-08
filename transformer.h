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

// wrap around a Hittable to perform a geometric rotation on it along the Y axis counter-clockwise
// use negative angle for clockwise.
// with this class, you can define Hittables with their dimensions without thinking about their rotation,
// and even re-use the same Hittable, but apply different transformations for it in the same scene.
// the trick is that it doesn't actually manipulate the points on the object, instead
// it moves the ray itself in the opposite direction, shoots the ray onto just that object
// to calculate whether there is an intersection and where, and then reverses the transformation
// on the intersection point and returns that, so the caller is none the wiser of what happened.
class RotateYTransformer : public Hittable {
    public:
        RotateYTransformer(std::shared_ptr<Hittable> const & target, double angle);

        virtual bool hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const override;

        virtual Aabb bounding_box() const override;

    private:
        // the sin of the rotation angle, cached for reuse
        double _sinTheta;
        // the cos of the rotation angle, cached for reuse
        double _cosTheta;

        std::shared_ptr<Hittable> _target;
        double _angle;
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

RotateYTransformer::RotateYTransformer(std::shared_ptr<Hittable> const & target, double angle)
                                       : _target(target), _angle(angle) {
    double radians = angle * PI / 180.0;

    this->_sinTheta = sin(radians);
    this->_cosTheta = cos(radians);

    // we need to properly rotate the bounding box since can't change how the
    // ray is checked against that like we do with the target itself
    this->_boundingBox = target->bounding_box();

    // this is a concise way of looping through every possible combination of lower and upper value
    // for the x, y and z of the bounding box, essentially going through all 6 corners of the box
    // and rotating them then finding the min and max points and put them in a AABB

    auto infinity = std::numeric_limits<double>::infinity();

    // the min point of the new bounding box that we'll be calculating post rotation
    Point3 min = Point3(infinity, infinity, infinity);
    // the max point of the new bounding box that we'll be calculating post rotation
    Point3 max = Point3(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                // the trick is we alternate between 0 and 1 for every axis
                // when its 0 it'll cancel out the "max" part of the interval and the (1 - i) will
                // invert the 0 to a 1 allowing the "min" part of the interval to be the value of the axis
                // do that but for every combination, and you've essentially iterated through every
                // vertex of the box
                // the combinations are:
                // (min, min, min)
                // (min, min, max)
                // (min, max, min)
                // (min, max, max)
                // (max, min, min)
                // (max, min, max)
                // (max, max, min)
                // (max, max, max)
                double x = (i * this->_boundingBox.xBounds.max) + ((1 - i) * this->_boundingBox.xBounds.min);
                double y = (j * this->_boundingBox.yBounds.max) + ((1 - j) * this->_boundingBox.yBounds.min);
                double z = (k * this->_boundingBox.zBounds.max) + ((1 - k) * this->_boundingBox.zBounds.min);

                double transformedX = (this->_cosTheta * x) + (this->_sinTheta * z);
                double transformedZ = (-this->_sinTheta * x) + (this->_cosTheta * z);

                min.x = fmin(min.x, transformedX);
                max.x = fmax(max.x, transformedX);

                min.y = fmin(min.y, y);
                max.y = fmax(max.y, y);

                min.z = fmin(min.z, transformedZ);
                max.z = fmax(max.z, transformedZ);
            }
        }
    }

    this->_boundingBox = Aabb(min, max);
}

bool RotateYTransformer::hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const {
    auto transformedOrigin = ray.orig;
    auto transformedDirection = ray.dir;

    transformedOrigin.x = (this->_cosTheta * ray.orig.x) - (this->_sinTheta * ray.orig.z);
    transformedOrigin.z = (this->_sinTheta * ray.orig.x) + (this->_cosTheta * ray.orig.z);

    transformedDirection.x = (this->_cosTheta * ray.dir.x) - (this->_sinTheta * ray.dir.z);
    transformedDirection.z = (this->_sinTheta * ray.dir.x) + (this->_cosTheta * ray.dir.z);

    auto transformedRay = Ray(transformedOrigin, transformedDirection, ray.time);

    if (this->_target->hit(transformedRay, rayLimits, result)) {
        // intersection found, now lets reverse the rotation on the point and normal

        auto intersectionPointCopy = result.point;
        result.point.x = (this->_cosTheta * intersectionPointCopy.x) + (this->_sinTheta * intersectionPointCopy.z);
        result.point.z = (-this->_sinTheta * intersectionPointCopy.x) + (this->_cosTheta * intersectionPointCopy.z);

        auto normalCopy = result.normal;
        result.normal.x = (this->_cosTheta * normalCopy.x) + (this->_sinTheta * normalCopy.z);
        result.normal.z = (-this->_sinTheta * normalCopy.x) + (this->_cosTheta * normalCopy.z);

        return true;
    } else {
        return false;
    }
}

Aabb RotateYTransformer::bounding_box() const {
    return this->_boundingBox;
}

#endif