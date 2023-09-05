#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"

// a representation of a medium that has constant probability of reflection as the ray travels through it
// unlike other objects which reflect at the surface only
// the boundary is whatever hittable objects that define the boundaries of the medium
// density controls how likely rays travelling through this medium are to reflect
// color/texture define how the medium affects the color of the ray as it travels through the medium
class ConstantMedium : public Hittable {
    public:
        ConstantMedium(std::shared_ptr<Hittable> const & boundary, double const & density, Color const & color);

        ConstantMedium(std::shared_ptr<Hittable> const & boundary, double const & density, std::shared_ptr<Texture> const & texture);

        virtual bool hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const override;

        virtual Aabb bounding_box() const override;

    private:
        std::shared_ptr<Hittable> _boundary;
        double _negativeInverseDensity;
        std::shared_ptr<Material> _mediumMaterial;
};

ConstantMedium::ConstantMedium(std::shared_ptr<Hittable> const & boundary, double const & density, Color const & color)
                               : ConstantMedium(boundary, density, std::make_shared<SolidColorTexture>(color)) { }

ConstantMedium::ConstantMedium(std::shared_ptr<Hittable> const & boundary, double const & density,
                               std::shared_ptr<Texture> const & texture)
                               : _boundary(boundary), _negativeInverseDensity(-1 / density),
                                 _mediumMaterial(std::make_shared<IsotropicScatterMaterial>(texture)) { }

// calculating hitting for this objects requires a few considerations. first it needs to actually hit the medium twice,
// once on entry, and once on exit. then we need to account for the fact that this ray could actually start
// inside the medium. also, that the point of reflection is not the same as the point of intersection.
bool ConstantMedium::hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const {
    HitResult mediumEntryHitResult, mediumExitHitResult;

    if (!this->_boundary->hit(ray, Interval::universe, mediumEntryHitResult)) {
        LOG(
            std::clog << "Ray completely missed constant medium " << mediumEntryHitResult.t << "\n";
        );
        return false;
    }

    LOG(
        std::clog << "Ray hit constant medium first at " << mediumEntryHitResult.t << "\n";
    );

    // Its always +0.001 rather than - cause we're always wanting to check for the second collision
    // in the direction of the ray.
    if (!this->_boundary->hit(ray, Interval(mediumEntryHitResult.t + 0.001, Interval::universe.max), mediumExitHitResult)) {
        LOG(
            std::clog << "Ray doesn't hit constant medium again in this direction\n";
        );
        return false;
    }

    LOG(
        std::clog << "Ray hit constant medium again at " << mediumExitHitResult.t << "\n";
    );

    // apply the rayLimits
    if (mediumEntryHitResult.t < rayLimits.min) mediumEntryHitResult.t = rayLimits.min;
    if (mediumExitHitResult.t > rayLimits.max) mediumExitHitResult.t = rayLimits.max;

    // as a result of the above, if the medium is behind the ray, then the entry point (which would actually
    // be rayLimit.min in that scenario) will be ahead of the exit point.
    if (mediumEntryHitResult.t >= mediumExitHitResult.t) {
        LOG(
            std::clog << "Ray t values after cropping are min: " << mediumEntryHitResult.t << ", max: "
                      << mediumExitHitResult.t << ", so it is therefore behind the ray\n";
        );
        return false;
    }

    if (mediumEntryHitResult.t < 0)
        mediumEntryHitResult.t = 0;

    double rayLength = ray.dir.length();
    double distanceWithinIntersectionPoints = (mediumExitHitResult.t - mediumEntryHitResult.t) * rayLength;
    // find a random number that reflects how far along the medium entry/exit points line did the ray get reflected
    // this is technically flawed in that we could get 0 from the random_double and fall apart
    double distanceTillReflection = this->_negativeInverseDensity * log(random_double());

    if (distanceTillReflection > distanceWithinIntersectionPoints) {
        LOG(
            std::clog << "Ray traversing constant medium would've reflected at " << distanceTillReflection
                      << " from the entry point, which is further than distance between points "
                      << distanceWithinIntersectionPoints << "\n";
        );
        return false;
    }

    result.t = mediumEntryHitResult.t + (distanceTillReflection / rayLength);
    result.point = ray.at(result.t);
    result.material = this->_mediumMaterial;
    // the following fields don't make sense/aren't relevant in this scenario
    result.isFrontFace = true;
    result.normal = Vec3(0, 0, 0);
    result.u = 0;
    result.v = 0;

    return true;
}

Aabb ConstantMedium::bounding_box() const {
    return this->_boundary->bounding_box();
}

#endif
