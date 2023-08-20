#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <ostream>

struct Vec3 {
    double x;
    double y;
    double z;

    Vec3() : x(0), y(0), z(0) { }

    Vec3(double i, double j, double k)
        : x(i), y(j), z(k) { }

    inline Vec3 operator+(Vec3 const & right) const {
        return Vec3(this->x + right.x, this->y + right.y, this->z + right.z);
    }

    inline Vec3 operator-() const {
        return Vec3(-this->x, -this->y, -this->z);
    }

    inline Vec3 operator-(Vec3 const & right) const {
        return Vec3(this->x - right.x, this->y - right.y, this->z - right.z);
    }

    inline Vec3 operator*(double const constant) const {
        return Vec3(constant * this->x, constant * this->y, constant * this->z);
    }

    inline Vec3 operator/(double const constant) const {
        return *this * (1 / constant);
    }

    double length_squared() const {
        return (x * x) + (y * y) + (z * z);
    }

    double length() const {
        return std::sqrt(this->length_squared());
    }

    Vec3 unit() const {
        return *this / this->length();
    }

    bool near_zero() const {
        auto const granularity = 1e-8;
        return (fabs(x) < granularity) && (fabs(y) < granularity) && (fabs(z) < granularity);
    }

    // reflects this Vec3 along the normal vector given
    // reflection of a vector v is v - 2(the projection of v onto the normal)
    // which leads to the equation v - 2(v.n)*n
    Vec3 reflect(Vec3 const & normal) {
        return *this - (normal * (2 * this->dot(normal)));
    }

    double dot(Vec3 const & right) const {
        return (this->x * right.x) + (this->y * right.y) + (this->z * right.z);
    }
};

// specific overload for when constant is on the left hand side of the operator
// so technically this is an overload for double
inline Vec3 operator*(double const left, Vec3 const & right) {
    return right * left;
}

inline std::ostream & operator<<(std::ostream & out, Vec3 const & v) {
    return out << v.x << " " << v.y << " " << v.z;
}

using Point3 = Vec3;

#endif
