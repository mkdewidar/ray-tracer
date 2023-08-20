#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "logger.h"

struct HitResult;

class Material {
    public:
        // Given a incoming ray and where it hit on a material, return the scattered ray and attentuation
        // attenuation is how much of the reflection's color should affect the final color
        virtual bool scatter(Ray const & incomingRay, HitResult const & result, Color & attenuation, Ray & scatteredRay) const = 0;
};

// Lambertian diffuse material
class LambertianMaterial : public Material {
    public:
        LambertianMaterial(Color const & a) : albedo(a) { }

        virtual bool scatter(Ray const & incomingRay, HitResult const & result, Color & attenuation, Ray & scatteredRay) const override {
            // we're imagining that there is a sphere where the normal vector is
            // the radius, then we get a random unit vector from there. the vector
            // from our original hit point to the new point is the direction of the reflection
            // originally that is: (result.point + result.normal + random_unit_vec3()) - result.point
            // but that simplifies to whats below
            auto reflectedRayDirection = result.normal + random_unit_vec3();

            // in case the randomly generated vector matches the normal and cancels it out
            if (reflectedRayDirection.near_zero())
                reflectedRayDirection = result.normal;

            LOG(
                std::clog << "Lambertian reflected ray direction: " << reflectedRayDirection << "\n";
            )

            // here's an alternative diffuse method that is mentioned by the book as well
            // auto reflectedRay = Ray(result.point, random_in_hemisphere(result.normal));

            scatteredRay = Ray(result.point, reflectedRayDirection);
            attenuation = this->albedo;

            return true;
        }

    public:
        Color albedo;
};

// Shiny metallic material
class MetalMaterial : public Material {
    public:
        MetalMaterial(Color const & a, double const f) : albedo(a), fuzz(f < 1 ? f : 1) { }

        virtual bool scatter(Ray const & incomingRay, HitResult const & result, Color & attenuation, Ray & scatteredRay) const override {
            auto reflectedRayDirection = incomingRay.dir.unit().reflect(result.normal);

            scatteredRay = Ray(result.point, reflectedRayDirection + (fuzz * random_unit_vec3_in_unit_sphere()));
            attenuation = this->albedo;

            LOG(
                std::clog << "Metallic reflected ray direction " << reflectedRayDirection << "\n";
            )

            // depending on the angle that the incoming ray was at, the reflection needs to either be
            // considered or ignored
            // if the angle is steep enough that the projection of the incoming ray/reflected ray onto the normal
            // gives less than
            return (reflectedRayDirection.dot(result.normal)) > 0;
        }

    public:
        Color albedo;
        double fuzz;
};

#endif