#ifndef RANDOM_H
#define RANDOM_H

#include <random>

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + ((max - min) * random_double());
}

// a unit sphere is a sphere with radius 1, so basically this creates a random
// unit vector where x, y and z could range between -1 and 1
//
// from what I understand, for the purpose of generating vectors relative to a normal,
// the random values produced by this follow a distribution
// of (cos(a))^3 where a is the angle between the randomly generated vector and the normal
// in other words, this way of generating a random vector is biased towards
// vectors that are closer to the normal.
inline Vec3 random_unit_vec3_in_unit_sphere() {
    while (true) {
        // the individual dimensions of a unit vector should be in the range [-1, 1]
        // though note in this case due to how the random generation works its
        // actually [-1, 1)
        auto p = Vec3(random_double(-1, 1), random_double(-1, 1), random_double(-1, 1));
        if (p.length_squared() >= 1) {
            continue;
        }
        return p;
    }
}

// when used for generating vectors relative to a normal, this apparently
// generates unit vectors that follow true Lambertian distribution
// which improves the likelihood of having vectors that have a larger angle away
// from said normal
inline Vec3 random_unit_vec3() {
    return random_unit_vec3_in_unit_sphere().unit();
}

// generates a random vector that is in the same direction as the normal
// it does this by generating a random vector in a sphere, and if what we generated
// is in the opposite direction (i.e wrong "hemisphere") then we inverse it and use
// it (rather than throw it away and try again)
inline Vec3 random_in_hemisphere(Vec3 const & normal) {
    Vec3 in_unit_sphere = random_unit_vec3_in_unit_sphere();
    if (in_unit_sphere.dot(normal) > 0.0) {
        return in_unit_sphere;
    } else {
        return -in_unit_sphere;
    }
}

// generates a random point that falls within a disk of radius 1
Point3 random_point_in_unit_disk() {
    while (true) {
        auto p = Point3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1.0) continue;
        return p;
    }
}

#endif