#ifndef VEC3_H
#define VEC3_H

#include <cmath>

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

    double dot(Vec3 const & right) const {
        return (this->x * right.x) + (this->y * right.y) + (this->z * right.z);
    }
};

// specific overload for when constant is on the left hand side of the operator
// so technically this is an overload for double
inline Vec3 operator*(double const left, Vec3 const & right) {
    return right * left;
}

using Point3 = Vec3;

#endif
