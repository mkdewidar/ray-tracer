#include <iostream>
#include <thread>

#include "vec3.h"
#include "ray.h"
#include "random.h"
#include "logger.h"


using namespace std::chrono_literals;


//         ^ y
//         |
//         |
//        /---> x
//       /
//      z (i.e positive z is out of the screen towards you)

int main() {
    // output image
    auto const aspectRatio = 16.0 / 9.0;

    int const imageWidth = 400;
    int const imageHeight = static_cast<int>(imageWidth / aspectRatio);

    std::clog << "Image width: " << imageWidth << ", height: " << imageHeight << "\n";

    // viewport and camera

    // viewport is our window to the world, if you imagine looking through a window
    // or a pair of glasses, the viewport is the glass, and the camera is the eye

    auto viewportHeight = 2.0;
    // we don't use aspectRatio because it might not be the real aspect ratio of the image, since the image's dimensions
    // are ints but the aspect ratio is a real number,
    // casting one of the image dimensions to a double first ensures we use double division instead of int division,
    // and therefore aren't prematurely truncating any real numbers
    auto viewportWidth = (static_cast<double>(imageWidth) / imageHeight) * viewportHeight;
    // the distance between the eye and the glass
    auto focalLength = 1.0;

    std::clog << "Viewport width: " << viewportWidth << ", height: " << viewportHeight
              << ", focal length: " << focalLength << "\n";

    auto origin = Point3(0, 0, 0);
    // a vector that's the same length as the viewport's width and points only
    // in the x axis for use later when traversing the scan lines
    auto horizontal = Vec3(viewportWidth, 0, 0);
    // a vector that's the same length as the viewport's height and points only
    // in the y axis for use later when traversing the scan lines
    auto vertical = Vec3(0, viewportHeight, 0);
    // lower left corner of viewport, in combination with the vectors above and
    // some other information in the rendering loop, we can traverse the viewport
    // from left to right, and up to down
    auto lowerLeftCorner = origin - horizontal/2 - vertical/2 - Vec3(0, 0, focalLength);

    std::clog << "Origin (center of viewport): " << origin
              << "\nHorizontal vector: " << horizontal
              << "\nVertical vector: " << vertical
              << "\nLower left corner: " << lowerLeftCorner << "\n";

    // Anti-aliasing samples per pixel
    auto const aaSamples = 10;
    // Number of reflections/bounces we can make off objects
    auto const maxDepth = 50;

    auto objects = std::vector<std::unique_ptr<Hittable>>();
    objects.push_back(std::make_unique<Sphere>(Vec3(0, 0, -1.0), 0.5));
    objects.push_back(std::make_unique<Sphere>(Vec3(0, -101, -1.0), 100));

    std::clog << "World contains objects: \n";
    for (auto & obj : objects) {
        std::clog << "- " << obj << "\n";
    }

    std::clog << "\n" << std::flush;

    // rendering
    std::cout << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

    // from top to bottom, left to right
    for (int j = imageHeight - 1; j >= 0; --j) { // from height - 1 -> 0
        std::clog << "\rScanlines remaining: " << j << std::flush;

        // uncomment the line below to slow the rendering and see the progress bar
        // std::this_thread::sleep_for(50ms);

        for (int i = 0; i < imageWidth; ++i) { // from 0 -> width - 1
            LOGGER_ENABLED = false;
            // uncomment the lines below and insert the pixel values for the rectangle you wish to debug
            // and all log lines will be printed during the calculation of that pixel value
            // if ((j >= 110) && (j <= 113)) {
            //     if ((i >= 120) && (i <= 210)) {
            //         LOGGER_ENABLED = true;
            //         std::clog << "\n" << "----" << "\n";
            //     }
            // }

            LOG(
                std::clog << "Pixel " << i << " " << j << "\n";
            )

            // this anti-aliasing implementation relies on taking random samples
            // of color and average them all to get the color for this pixel
            Color cumulativeColor = Color(0, 0, 0);
            for (int s = 0; s < aaSamples; ++s) {
                // a scalar value that is used to shorten the "horizontal" vector to
                // the point on the viewport we are currently rendering
                double u = (double(i) + random_double()) / (imageWidth - 1);
                // a scalar value that is used to shorten the "vertical" vector to
                // the point on the viewport we are currently rendering
                double v = (double(j) + random_double()) / (imageHeight - 1);

                // origin may not be zero (if camera moved location), but the direction
                // we would have calculated would be relative to true origin. The - origin
                // at the end makes the direction relative to whatever the camera's location is
                auto r = Ray(origin, lowerLeftCorner + (u * horizontal) + (v * vertical) - origin);

                cumulativeColor = cumulativeColor + ray_color(r, objects, maxDepth);
            }

            // this "gamma corrects" the colors, which I don't really understand
            // and my image was already lighter than the guide for some reason
            auto gammaCorrected = Color(
                sqrt(cumulativeColor.r() / aaSamples),
                sqrt(cumulativeColor.g() / aaSamples),
                sqrt(cumulativeColor.b() / aaSamples));

            write_color(std::cout, gammaCorrected);
        }
    }

    std::clog << "\nDone\n";

    return 0;
}
