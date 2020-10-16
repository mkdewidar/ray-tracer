#ifndef VEC3_H
#define VEC3_H

struct Vec3 {
    double x;
    double y;
    double z;

    Vec3() { }

    Vec3(double i, double j, double k)
        : x(i), y(j), z(k) { }

    inline Vec3 operator+(Vec3 const & right) const {
        return Vec3(this->x + right.x, this->y + right.y, this->z + right.z);
    }

    inline Vec3 operator*(double const constant) const {
        return Vec3(constant * this->x, constant + this->y, constant * this->z);
    }
};

// specific overload for when constant is on the left hand side of the operator
// so technically this is an overload for double
inline Vec3 operator*(double const left, Vec3 const & right) {
    return right * left;
}


#endif
