#ifndef COLOR_H
#define COLOR_H

#include <ostream>
#include <cassert>

#include "logger.h"
#include "interval.h"

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
    auto intensityLimit = Interval(0.000000, 0.999999);

    // don't want to actually get 256 as a color value, we want to stop at 255
    // as for the sqrt, that's "gamma correction". The color we receive into this function
    // is in "linear space", whereas most image viewing programs expect color to be in "gamma space",
    // where the spacing between color values is not even. Doing the sqrt of a color value converts
    // our colors to "gamma 2" space. See https://docs.unity3d.com/Manual/LinearLighting.html for more info.
    int R = static_cast<int>(intensityLimit.clamp(sqrt(c.r)) * 256);
    int G = static_cast<int>(intensityLimit.clamp(sqrt(c.g)) * 256);
    int B = static_cast<int>(intensityLimit.clamp(sqrt(c.b)) * 256);

    LOG(
        std::clog << "Raw color: " << c.r << " " << c.g << " " << c.b << "\n";
        std::clog << "Actual color: " << R << " " << G << " " << B << "\n";
    )

    output << R << " " << G << " " << B << "\n";
}

#endif
