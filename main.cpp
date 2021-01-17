#include <iostream>
#include <thread>

#include "vec3.h"
#include "ray.h"


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

    // viewport and camera

    // viewport is our window to the world, if you imagine looking through a window
    // or a pair of glasses, the viewport is the glass, and the camera is the eye

    auto viewportHeight = 2.0;
    auto viewportWidth = aspectRatio * viewportHeight;
    // the distance between the eye and the glass
    auto focalLength = 1.0;

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

    // rendering
    std::cout << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

    for (int j = imageHeight - 1; j >= 0; --j) { // from height - 1 -> 0
        std::cerr << "\rScanlines remaining: " << j << std::flush;

        // uncomment the line below to slow the rendering and see the progress bar
        // std::this_thread::sleep_for(50ms);

        for (int i = 0; i < imageWidth; ++i) { // from 0 -> width - 1
            // a scalar value that is used to shorten the "horizontal" vector to
            // the point on the viewport we are currently rendering
            double u = double(i) / (imageWidth - 1);
            // a scalar value that is used to shorten the "vertical" vector to
            // the point on the viewport we are currently rendering
            double v = double(j) / (imageHeight - 1);

            // origin may not be zero (if camera moved location), but the direction
            // we would have calculated would be relative to true origin. The - origin
            // at the end makes the direction relative to whatever the camera's location is
            auto r = Ray(origin, lowerLeftCorner + (u * horizontal) + (v * vertical) - origin);

            Color c = ray_color(r);

            write_color(std::cout, c);
        }
    }

    std::cerr << "\nDone\n";

    return 0;
}
