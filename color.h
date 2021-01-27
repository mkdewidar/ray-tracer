#ifndef COLOR_H
#define COLOR_H

#include <ostream>
#include <cassert>

struct Color {
    Color() { }

    Color(double red, double green, double blue)
        : _r(red), _g(green), _b(blue) { }

    inline Color operator+(Color const & right) const {
        return Color(this->_r + right.r(), this->_g + right.g(), this->_b + right.b());
    }

    inline Color operator-(Color const & right) const {
        return Color(this->_r - right.r(), this->_g - right.g(), this->_b - right.b());
    }

    inline Color operator*(double const constant) const {
        return Color(constant * this->_r, constant * this->_g, constant * this->_b);
    }

    inline Color operator/(double const constant) const {
        return Color(this->_r / constant, this->_g / constant, this->_b / constant);
    }

    inline double r() const {
        return this->_r;
    }

    inline double g() const {
        return this->_g;
    }

    inline double b() const {
        return this->_b;
    }

    inline void r(double red) {
        this->_r = red;
    }

    inline void g(double green) {
        this->_g = green;
    }

    inline void b(double blue) {
        this->_b = blue;
    }

private:
    double _r;
    double _g;
    double _b;
};

// specific overload for when constant is on the left hand side of the operator
// so technically this is an overload for double
inline Color operator*(double const left, Color const & right) {
    return right * left;
}

void write_color(std::ostream & output, Color const & c) {
    int R = static_cast<int>(c.r() * 255.999); // the .999 allows us
    int G = static_cast<int>(c.g() * 255.999); // to make sure 256 is
    int B = static_cast<int>(c.b() * 255.999); // as likely to occur as other colors

    output << R << " " << G << " " << B << "\n";
}

#endif
