#ifndef IMAGE_H
#define IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "external/stb_image.h"

#include <string>
#include <iostream>

class Image {
public:
    Image(std::string const & filename);

    ~Image();

    double * const color_at(int x, int y) const;

    int width = 0;
    int height = 0;
private:
    // the color values for the image, a [0-1] for each component of a pixel,
    // so overall there will be width * height * components per pixel parts to this array
    double * colorData = nullptr;

    // total number of color components (i.e r, g, b) per row of image, used for color value lookups
    int componentsPerRow = 0;

    // for when the image couldn't be loaded
    static double FALLBACK_COLOR[];

    static int const COMPONENTS_PER_PIXEL = 3;
};

// ------

double Image::FALLBACK_COLOR[] = {255, 0, 0};

Image::Image(std::string const & filename) {
    int imageBytesPerPixel = 0;
    float* floatData = stbi_loadf(filename.c_str(), &width, &height, &imageBytesPerPixel, COMPONENTS_PER_PIXEL);

    if (floatData == nullptr) {
        std::cerr << "Error loading image " << filename << ", reason: " << stbi_failure_reason() << std::endl;
        return;
    }

    std::clog << "Loaded image " << filename <<
              ". Width: " << width << ", height: " << height << ", bytes per pixel: " << imageBytesPerPixel
              << "\n";

    int totalBytes = height * width * COMPONENTS_PER_PIXEL;

    colorData = new double[totalBytes];

    for (int i = 0; i < totalBytes; i++) {
        colorData[i] = static_cast<double>(floatData[i]);
    }

    componentsPerRow = COMPONENTS_PER_PIXEL * width;

    STBI_FREE(floatData);
}

Image::~Image() {
    delete[] colorData;
}

static int clamp_within(int x, int lowInclusive, int highExclusive) {
    if (x < lowInclusive) return lowInclusive;
    if (x < highExclusive) return x;
    return highExclusive - 1;
}

double * const Image::color_at(int x, int y) const {
    if (this->colorData == nullptr) {
        return FALLBACK_COLOR;
    }

    x = clamp_within(x, 0, width);
    y = clamp_within(y, 0, height);

    return colorData + (x * COMPONENTS_PER_PIXEL) + (y * componentsPerRow);
}


#endif