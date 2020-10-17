#ifndef COLOR_H
#define COLOR_H

#include <ostream>

struct Color {
    double r;
    double g;
    double b;

    Color() { }

    Color(double red, double green, double blue)
        : r(red), g(green), b(blue) { }

    inline Color operator+(Color const & right) const {
        return Color(this->r + right.r, this->g + right.g, this->b + right.b);
    }

    inline Color operator-(Color const & right) const {
        return Color(this->r - right.r, this->g - right.g, this->b - right.b);
    }

    inline Color operator*(double const constant) const {
        return Color(constant * this->r, constant * this->g, constant * this->b);
    }
};

// specific overload for when constant is on the left hand side of the operator
// so technically this is an overload for double
inline Color operator*(double const left, Color const & right) {
    return right * left;
}

void write_color(std::ostream & output, Color const & c) {
    int R = static_cast<int>(c.r * 255.999); // the .999 allows us
    int G = static_cast<int>(c.g * 255.999); // to make sure 256 is
    int B = static_cast<int>(c.b * 255.999); // as likely to occur as other colors

    output << R << " " << G << " " << B << "\n";
}

#endif
