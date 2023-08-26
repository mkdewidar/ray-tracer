#ifndef COLOR_H
#define COLOR_H

#include <ostream>
#include <cassert>

#include "logger.h"

class Color {
    public:
        double r;
        double g;
        double b;

        Color();

        Color(double red, double green, double blue);

        Color operator+(Color const & right) const;

        Color operator-(Color const & right) const;

        Color operator*(double const constant) const;

        Color operator*(Color const & right) const;

        Color operator/(double const constant) const;
};

Color operator*(double left, Color const & right);

void write_color(std::ostream & output, Color const & c);

// ------

Color::Color() : r(0), g(0), b(0) { }

Color::Color(double red, double green, double blue) : r(red), g(green), b(blue) { }

Color Color::operator+(Color const & right) const {
    return Color(this->r + right.r, this->g + right.g, this->b + right.b);
}

Color Color::operator-(Color const & right) const {
    return Color(this->r - right.r, this->g - right.g, this->b - right.b);
}

Color Color::operator*(double const constant) const {
    return Color(constant * this->r, constant * this->g, constant * this->b);
}

Color Color::operator*(Color const & right) const {
    return Color(this->r * right.r, this->g * right.g, this->b * right.b);
}

Color Color::operator/(double const constant) const {
    return Color(this->r / constant, this->g / constant, this->b / constant);
}

// specific overload for when constant is on the left hand side of the operator
// so technically this is an overload for double
Color operator*(double left, Color const & right) {
    return right * left;
}

void write_color(std::ostream & output, Color const & c) {
    int R = static_cast<int>(c.r * 255.999); // the .999 allows us
    int G = static_cast<int>(c.g * 255.999); // to make sure 256 is
    int B = static_cast<int>(c.b * 255.999); // as likely to occur as other colors

    LOG(
        std::clog << "Raw color: " << c.r << " " << c.g << " " << c.b << "\n";
        std::clog << "Actual color: " << R << " " << G << " " << B << "\n";
    )

    output << R << " " << G << " " << B << "\n";
}

#endif
