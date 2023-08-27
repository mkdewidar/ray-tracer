#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <memory>
#include <vector>

#include "hittable.h"
#include "aabb.h"

// a container for hittable objects
class HittableList : public Hittable {
    public:
        std::vector<std::shared_ptr<Hittable>> objects;
        Aabb boundingBox;

        HittableList();
        HittableList(std::shared_ptr<Hittable> objects);

        void clear();

        void add(std::shared_ptr<Hittable> object);

        bool hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const override;

        Aabb bounding_box() const override;
};

std::ostream & operator<<(std::ostream & out, HittableList const & list);

// ------

HittableList::HittableList() : objects(), boundingBox() { }

HittableList::HittableList(std::shared_ptr<Hittable> object) {
    add(object);
}

void HittableList::clear() {
    this->objects.clear();
}

void HittableList::add(std::shared_ptr<Hittable> object) {
    this->objects.push_back(object);
    this->boundingBox = Aabb(this->boundingBox, object->bounding_box());
}

bool HittableList::hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const {
    double maxRayLength = rayLimits.max; // essentially our view distance

    bool didHitAnything = false;

    // for use inside the loop as an output parameter to the hit function
    auto tmpHitResult = HitResult();

    for (std::shared_ptr<Hittable> const & object : this->objects) {

        if (object->hit(ray, Interval(rayLimits.min, maxRayLength), tmpHitResult)) {
            result = tmpHitResult;

            // the t for object becomes our new max length of the ray
            // allowing us to ensure we pick the color of objects that are closest to us
            maxRayLength = result.t;

            didHitAnything = true;
            LOG(
                std::clog << "Ray hit object " << object << " with t = " << result.t << ", front face: " << result.isFrontFace << "\n";
                std::clog << "Normal: " << result.normal.x << " " << result.normal.y << " " << result.normal.z << "\n";
            )
        }
    }

    return didHitAnything;
}

Aabb HittableList::bounding_box() const {
    return this->boundingBox;
}

std::ostream & operator<<(std::ostream & out, HittableList const & list) {
    for (auto & obj : list.objects) {
        out << "- " << obj << "\n";
    }

    return out;
}

#endif
