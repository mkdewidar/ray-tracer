#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <ostream>

class Vec3 {
    public:
        double x;
        double y;
        double z;

        Vec3();

        Vec3(double i, double j, double k);

        Vec3 operator+(Vec3 const & right) const;

        Vec3 operator-() const;

        Vec3 operator-(Vec3 const & right) const;

        Vec3 operator*(double const constant) const;

        Vec3 operator/(double const constant) const;

        double length_squared() const;

        double length() const;

        Vec3 unit() const;

        bool near_zero() const;

        Vec3 reflect(Vec3 const & normal);

        Vec3 refract(Vec3 const & normal, double const refractiveIndexRatio);

        double dot(Vec3 const & right) const;

        Vec3 cross(Vec3 const & right) const;
};

Vec3 operator*(double left, Vec3 const & right);

std::ostream & operator<<(std::ostream & out, Vec3 const & v);

using Point3 = Vec3;

// ------

Vec3::Vec3() : x(0), y(0), z(0) { }

Vec3::Vec3(double i, double j, double k) : x(i), y(j), z(k) { }

Vec3 Vec3::operator+(Vec3 const & right) const {
    return Vec3(this->x + right.x, this->y + right.y, this->z + right.z);
}

Vec3 Vec3::operator-() const {
    return Vec3(-this->x, -this->y, -this->z);
}

Vec3 Vec3::operator-(Vec3 const & right) const {
    return Vec3(this->x - right.x, this->y - right.y, this->z - right.z);
}

Vec3 Vec3::operator*(double const constant) const {
    return Vec3(constant * this->x, constant * this->y, constant * this->z);
}

Vec3 Vec3::operator/(double const constant) const {
    return *this * (1 / constant);
}

double Vec3::length_squared() const {
    return (x * x) + (y * y) + (z * z);
}

double Vec3::length() const {
    return std::sqrt(this->length_squared());
}

Vec3 Vec3::unit() const {
    return *this / this->length();
}

bool Vec3::near_zero() const {
    auto const granularity = 1e-8;
    return (fabs(x) < granularity) && (fabs(y) < granularity) && (fabs(z) < granularity);
}

// reflects this Vec3 along the normal vector given
// reflection of a vector v is v - 2(the projection of v onto the normal)
// which leads to the equation v - 2(v.n)*n
Vec3 Vec3::reflect(Vec3 const & normal) {
    return *this - ((2 * this->dot(normal)) * normal);
}

// refraction is a ray coming into a material one angle from a normal, and
// leaving at another angle from the normal into the second material
// this function returns the refracted vector
// normal is the vector perpendicular to the material surface where the ray is coming,
// default direction is "upwards" from the material
// refractiveIndexRatio is the ratio of the refractive index of the outside material (usually air)
// over the inside material of the inside
// NOTE: "this" vector must be a unit vector when you use this function on it
Vec3 Vec3::refract(Vec3 const & normal, double const refractiveIndexRatio) {
    Vec3 thisVec = *this;
    // the component of the refracted vector that is perpendicular to the normal
    Vec3 refractedVectorPerp = (thisVec + (fmin((-thisVec).dot(normal), 1.0) * normal)) * refractiveIndexRatio;
    // the component of the refracted vector that is parallel to the normal
    Vec3 refractedVectorParallel = -sqrt(fabs(1.0 - refractedVectorPerp.length_squared())) * normal;
    return refractedVectorParallel + refractedVectorPerp;
}

double Vec3::dot(Vec3 const & right) const {
    return (this->x * right.x) + (this->y * right.y) + (this->z * right.z);
}

Vec3 Vec3::cross(Vec3 const & right) const {
    return Vec3((this->y * right.z) - (this->z * right.y),
                (this->z * right.x) - (this->x * right.z),
                (this->x * right.y) - (this->y * right.x));
}

// specific overload for when constant is on the left hand side of the operator
// so technically this is an overload for double
Vec3 operator*(double const left, Vec3 const & right) {
    return right * left;
}

std::ostream & operator<<(std::ostream & out, Vec3 const & v) {
    return out << v.x << " " << v.y << " " << v.z;
}

#endif
