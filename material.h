#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "logger.h"

class HitResult;

class Material {
    public:
        // Given a incoming ray and where it hit on a material, return the scattered ray and attentuation
        // attenuation is how much of the reflection's color should affect the final color
        virtual bool scatter(Ray const & incomingRay, HitResult const & result, Color & attenuation, Ray & scatteredRay) const = 0;
};

// Lambertian diffuse material
// an approach for modelling diffuse surfaces that includes bouncing rays that intersect with the object
// at angles that are closer to the normal
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
            if (reflectedRayDirection.is_near_zero())
                reflectedRayDirection = result.normal;

            LOG(
                std::clog << "Lambertian reflected ray direction: " << reflectedRayDirection << "\n";
            )

            // here's an alternative diffuse method that is mentioned by the book as well
            // which is based on just randomly reflecting in any direction away the the surface
            // irrespective of how we got there or our angle to the normal
            // although the difference creating random points in a hemisphere vs sphere seems simple,
            // it affects the distribution of the output vectors (hemisphere leads to uniform, sphere is
            // biased towards the normal)
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

class DielectricMaterial : public Material {
    public:
        DielectricMaterial(double const ri) : refractionIndex(ri) { }

        virtual bool scatter(Ray const & incomingRay, HitResult const & result, Color & attenuation, Ray & scatteredRay) const override {
            attenuation = Color(1.0, 1.0, 1.0);

            // assuming that the material on the "outside" is air
            double refractionIndexRatio = result.isFrontFace ? (1.0 / refractionIndex) : refractionIndex;

            Vec3 incomingRayDirUnit = incomingRay.dir.unit();

            // "theta" is the angle between the incoming ray and the normal
            double cosineTheta = fmin((-incomingRayDirUnit).dot(result.normal), 1.0);
            double sineTheta = sqrt(1.0 - (cosineTheta * cosineTheta));

            // if sineThetaPrime (which is the result of ratio * sineTheta) is greater than 1.0
            // then Snell's law has no real solution and we cannot refract, and instead perform total internal reflection
            bool cannotRefract = (refractionIndexRatio * sineTheta) > 1.0;

            Vec3 outVector;
            if (cannotRefract || (reflectance(cosineTheta, refractionIndexRatio) > random_double())) {
                outVector = incomingRayDirUnit.reflect(result.normal);
            } else {
                outVector = incomingRayDirUnit.refract(result.normal, refractionIndexRatio);
            }

            scatteredRay = Ray(result.point, outVector);
            return true;
        }

    public:
        double refractionIndex;

    private:
        // an implementation of Schlick's approximation, which approximates how much of the incoming ray is reflected back
        static double reflectance(double cosineTheta, double refractiveIndex) {
            auto r0 = (1 - refractiveIndex) / (1 + refractiveIndex);
            r0 = r0 * r0;
            return r0 + ((1 - r0) * pow((1 - cosineTheta), 5));
        }
};

#endif