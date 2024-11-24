#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include "image.h"

class Texture {
    public:
        virtual ~Texture() = default;

        virtual Color value(double u, double v, Point3 const & p) const = 0;
};

class SolidColorTexture : public Texture {
    public:
        SolidColorTexture(Color c);
        SolidColorTexture(double red, double green, double blue);

        Color value(double u, double v, Point3 const & p) const override;

    private:
        Color _color;
};

class CheckeredTexture : public Texture {
    public:
        CheckeredTexture(double scale, Color const & odd, Color const & even);
        CheckeredTexture(double scale, std::shared_ptr<Texture> const & odd, std::shared_ptr<Texture> const & even);

        Color value(double u, double v, Point3 const & p) const override;

    private:
        double _invertScaleFactor;
        std::shared_ptr<Texture> _oddPatternTexture;
        std::shared_ptr<Texture> _evenPatternTexture;
};

class ImageTexture : public Texture {
    public:
        ImageTexture(std::string const & filename);

        Color value(double u, double v, Point3 const & p) const override;
    private:
        Image image;
};

// ------

SolidColorTexture::SolidColorTexture(Color c) : _color(c) { }

SolidColorTexture::SolidColorTexture(double red, double green, double blue) : _color(Color(red, green, blue)) { }

Color SolidColorTexture::value(double u, double v, Point3 const & p) const {
    return this->_color;
}


CheckeredTexture::CheckeredTexture(double scale, Color const & odd, Color const & even)
                                   : _invertScaleFactor(1.0 / scale),
                                     _oddPatternTexture(std::make_shared<SolidColorTexture>(odd)),
                                     _evenPatternTexture(std::make_shared<SolidColorTexture>(even)) { }

CheckeredTexture::CheckeredTexture(double scale, std::shared_ptr<Texture> const & odd, std::shared_ptr<Texture> const & even)
                                   : _invertScaleFactor(1.0 / scale), _oddPatternTexture(odd), _evenPatternTexture(even) { }

Color CheckeredTexture::value(double u, double v, Point3 const & p) const {
    auto x = static_cast<int>(std::floor(p.x * this->_invertScaleFactor));
    auto y = static_cast<int>(std::floor(p.y * this->_invertScaleFactor));
    auto z = static_cast<int>(std::floor(p.z * this->_invertScaleFactor));

    auto isEven = ((x + y + z) % 2) == 0;

    return isEven ? _evenPatternTexture->value(u, v, p) : _oddPatternTexture->value(u, v, p);
}


ImageTexture::ImageTexture(std::string const & filename)
                                   : image(filename) { }

Color ImageTexture::value(double u, double v, Point3 const & p) const {
    u = Interval(0, 1).clamp(u);
    v = 1.0 - Interval(0, 1).clamp(v); // flip v because image is mapped from top to bottom

    double * const pixel = this->image.color_at(int(u * this->image.width), int(v * this->image.height));
    return Color(pixel[0], pixel[1], pixel[2]);
}

#endif