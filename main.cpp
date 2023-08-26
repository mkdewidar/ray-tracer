#include <iostream>
#include <thread>


#include "vec3.h"
#include "color.h"
#include "random.h"
#include "logger.h"

#include "ray.h"

#include "hittable.h"
#include "material.h"
#include "sphere.h"


using namespace std::chrono_literals;

double const PI = 3.1415926535897932385;

//         ^ y
//         |
//         |
//        /---> x
//       /
//      z (i.e positive z is out of the screen towards you)

int main() {
    // output image
    auto const aspectRatio = 16.0 / 9.0;

    int const imageWidth = 1920;
    int const imageHeight = static_cast<int>(imageWidth / aspectRatio);

    std::clog << "Image width: " << imageWidth << ", height: " << imageHeight << "\n";

    // camera

    auto fieldOfView = 90; // vertical
    auto cameraOrigin = Point3(3, 3, 2);
    auto cameraTarget = Point3(0, 0, -1);

    // the "z" axis of the camera
    auto w = (cameraOrigin - cameraTarget).unit();
    // the "x" axis of the camera
    // the cross product between up in the world, and "z" of the camera
    auto u = Vec3(0, 1, 0).cross(w).unit();
    // the "y" axis of the camera
    auto v = w.cross(u);

    double aperture = 2.0;
    double lensRadius = aperture / 2.0;

    std::clog << "Field Of View: " << fieldOfView << "\n"
              << "Camera Origin: " << cameraOrigin << "\n"
              << "Camera pointing at: " << cameraTarget << "\n"
              << "Camera up: " << v << "\n"
              << "Camera right: " << u << "\n"
              << "Camera back: " << w << "\n"
              << "Aperture: " << aperture << "\n";

    // viewport

    // viewport is our window to the world, if you imagine looking through a window
    // or a pair of glasses, the viewport is the glass, and the camera is the eye

    // 2.0 * tan(vfov in radians / 2)
    auto viewportHeight = 2.0 * (tan(fieldOfView * PI / 180.0 / 2));
    // we don't use aspectRatio because it might not be the real aspect ratio of the image, since the image's dimensions
    // are ints but the aspect ratio is a real number,
    // casting one of the image dimensions to a double first ensures we use double division instead of int division,
    // and therefore aren't prematurely truncating any real numbers
    auto viewportWidth = (static_cast<double>(imageWidth) / imageHeight) * viewportHeight;

    auto focusDistance = (cameraOrigin - cameraTarget).length();

    std::clog << "Viewport width: " << viewportWidth << ", height: " << viewportHeight
              << ", focus distance: " << focusDistance << "\n";

    // a vector that's the same length as the viewport's width and points only
    // in the x axis for use later when traversing the scan lines
    auto horizontal = viewportWidth * u * focusDistance;
    // a vector that's the same length as the viewport's height and points only
    // in the y axis for use later when traversing the scan lines
    auto vertical = viewportHeight * v * focusDistance;
    // lower left corner of viewport, in combination with the vectors above and
    // some other information in the rendering loop, we can traverse the viewport
    // from left to right, and up to down
    auto lowerLeftCorner = cameraOrigin - (horizontal / 2) - (vertical / 2) - (w * focusDistance);

    std::clog << "Horizontal vector: " << horizontal << "\n"
              << "Vertical vector: " << vertical << "\n"
              << "Lower left corner: " << lowerLeftCorner << "\n";

    // Anti-aliasing samples per pixel
    auto const aaSamples = 10;
    // Number of reflections/bounces we can make off objects
    auto const maxDepth = 50;

    auto objects = std::vector<std::unique_ptr<Hittable>>();

    // ground
    objects.push_back(std::make_unique<Sphere>(Vec3(0.0, -100.5, -1.0),
                                               100.0,
                                               std::make_shared<LambertianMaterial>(Color(0.8, 0.8, 0.0))));
    // center sphere
    objects.push_back(std::make_unique<Sphere>(Vec3(0.0, 0.0, -1.0),
                                               0.5,
                                               std::make_shared<LambertianMaterial>(Color(0.7, 0.3, 0.3))));
    // left sphere
    objects.push_back(std::make_unique<Sphere>(Vec3(-1.0, 0.0, -1.0),
                                               0.5,
                                               std::make_shared<DielectricMaterial>(1.5)));
    // left sphere, again, this time its inside the other left sphere, and also is inside out (negative radius)
    objects.push_back(std::make_unique<Sphere>(Vec3(-1.0, 0.0, -1.0),
                                               -0.45,
                                               std::make_shared<DielectricMaterial>(1.5)));
    // right sphere
    objects.push_back(std::make_unique<Sphere>(Vec3(1.0, 0.0, -1.0),
                                               0.5,
                                               std::make_shared<MetalMaterial>(Color(0.8, 0.6, 0.2), 1.0)));

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
                double horizontalScalar = (double(i) + random_double(0.0, 0.9)) / (imageWidth - 1);
                // a scalar value that is used to shorten the "vertical" vector to
                // the point on the viewport we are currently rendering
                double verticalScalar = (double(j) + random_double(0.0, 0.9)) / (imageHeight - 1);

                // to simulate depth of field, we have a disk lens from which light is sourced
                Point3 pointOnLens = lensRadius * random_point_in_unit_disk();
                Point3 pointOnLensOnCamera = (u * pointOnLens.x) + (v * pointOnLens.y);

                // cameraOrigin may not be zero (if camera moved location), but the direction
                // we would have calculated would be relative to true origin. The - origin
                // at the end makes the direction relative to whatever the camera's location is
                auto r = Ray(cameraOrigin + pointOnLensOnCamera,
                             lowerLeftCorner + (horizontalScalar * horizontal) + (verticalScalar * vertical) - cameraOrigin - pointOnLensOnCamera);

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
