#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "random.h"
#include "logger.h"
#include "ray.h"

class Camera {
    public:
        double aspectRatio = 16.0 / 9.0;
        int imageWidth = 1920;
        int imageHeight;

        int fieldOfView = 90; // vertical

        // the next three variables control the camera's position and rotation by controlling where its
        // looking and what is considered "up"

        // camera location in the world
        Point3 cameraOrigin = Point3(3, 0, 2);
        // the point that the camera origin is "looking" at, this point is on the "focus plane" and will
        // have perfect focus if depth of field is used
        Point3 cameraTarget = Point3(0, 0, -1);
        // the up vector for the camera that decides how much the camera is rotated along the Z axis
        // 0,1,0 means "up" for the camera matches "up" in the world so camera is not tilted
        Vec3 cameraViewUp = Vec3(0, 1, 0);

        // the aperture of the lens i.e diameter of the lens,
        // 0 means no depth of field and everything is in perfect focus
        double aperture = 0;

        // Anti-aliasing samples per pixel
        int aaSamples = 10;
        // Number of reflections/bounces we can make off objects
        int maxDepth = 50;

        void render(std::shared_ptr<Hittable> const & world, void (*postInitialize) (Camera const &), void (*writeColorCallback) (Color const &));

    private:
        // u, v, w are camera axis, which are different from the world axis if the camera is rotated

        // the "z" axis of the camera
        Vec3 _w;
        // the "x" axis of the camera
        // the cross product between up in the world, and "z" of the camera
        Vec3 _u;
        // the "y" axis of the camera
        Vec3 _v;

        double _lensRadius = aperture / 2.0;

        // viewport is our window to the world, if you imagine looking through a window
        // or a pair of glasses, the viewport is the glass, and the camera is the eye

        double _viewportHeight;
        double _viewportWidth;
        // the distance between the camera lens and the plane at which objects will be in focus (focus plane)
        // since the viewport is basically the focus plane, this is the distance to the viewport
        double _focusDistance;

        // a vector that's the same length as the viewport's width and points only
        // in the x axis for use later when traversing the scan lines
        Vec3 _horizontal;
        // a vector that's the same length as the viewport's height and points only
        // in the y axis for use later when traversing the scan lines
        Vec3 _vertical;
        // lower left corner of viewport, in combination with the vectors above and
        // some other information in the rendering loop, we can traverse the viewport
        // from left to right, and up to down
        Vec3 _lowerLeftCorner;

        Ray get_ray(int i, int j) const;

        void initialize();
};

// ------

void Camera::render(std::shared_ptr<Hittable> const & world, void (*postInitialize) (Camera const &), void (*writeColorCallback) (Color const &)) {
    initialize();

    postInitialize(*this);

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

                Ray r = get_ray(i, j);

                cumulativeColor = cumulativeColor + ray_color(r, world, maxDepth);
            }


            auto antiAliasedColor = Color(cumulativeColor.r / aaSamples,
                                          cumulativeColor.g / aaSamples,
                                          cumulativeColor.b / aaSamples);

            writeColorCallback(antiAliasedColor);
        }
    }

    std::clog << "\nDone\n";

}

Ray Camera::get_ray(int i, int j) const {
    // a scalar value that is used to shorten the "horizontal" vector to
    // the point on the viewport we are currently rendering
    double horizontalScalar = (double(i) + random_double(0.0, 0.9)) / (imageWidth - 1);
    // a scalar value that is used to shorten the "vertical" vector to
    // the point on the viewport we are currently rendering
    double verticalScalar = (double(j) + random_double(0.0, 0.9)) / (imageHeight - 1);

    // to simulate depth of field, we have a disk lens from which light is sourced
    Point3 pointOnLens = _lensRadius * random_point_in_unit_disk();
    Point3 pointOnLensOnCamera = (_u * pointOnLens.x) + (_v * pointOnLens.y);

    // cameraOrigin may not be zero (if camera moved location), but the direction
    // we would have calculated would be relative to true origin. The - origin
    // at the end makes the direction relative to whatever the camera's location is
    return Ray(cameraOrigin + pointOnLensOnCamera,
               _lowerLeftCorner + (horizontalScalar * _horizontal) + (verticalScalar * _vertical) - cameraOrigin - pointOnLensOnCamera,
               random_double(0, 1)); // randomising the moment in time that we're rendering is good enough for motion blur
}

void Camera::initialize() {
    imageHeight = static_cast<int>(imageWidth / aspectRatio);

    std::clog << "Image width: " << imageWidth << ", height: " << imageHeight << ", aspect ratio: " << aspectRatio << "\n";

    // w is the opposite of where we're looking (to be consistent with same right hand system as world)
    _w = (cameraOrigin - cameraTarget).unit();
    // u is perpendicular to both "up" and w
    _u = cameraViewUp.cross(_w).unit();
    // v is perpendicular to both u and w
    _v = _w.cross(_u);

    std::clog << "Field Of View: " << fieldOfView << "\n"
              << "Camera Origin: " << cameraOrigin << "\n"
              << "Camera pointing at: " << cameraTarget << "\n"
              << "Camera up: " << _v << "\n"
              << "Camera right: " << _u << "\n"
              << "Camera back: " << _w << "\n"
              << "Aperture: " << aperture << "\n";

    // 2.0 * tan(vfov in radians / 2)
    _viewportHeight = 2.0 * (tan(fieldOfView * PI / 180.0 / 2));
    // we don't use aspectRatio because it might not be the real aspect ratio of the image, since the image's dimensions
    // are ints but the aspect ratio is a real number,
    // casting one of the image dimensions to a double first ensures we use double division instead of int division,
    // and therefore aren't prematurely truncating any real numbers
    _viewportWidth = (static_cast<double>(imageWidth) / imageHeight) * _viewportHeight;

    _focusDistance = (cameraOrigin - cameraTarget).length();

    std::clog << "Viewport width: " << _viewportWidth << ", height: " << _viewportHeight
              << ", focus distance: " << _focusDistance << "\n";

    _horizontal = _viewportWidth * _u * _focusDistance;
    _vertical = _viewportHeight * _v * _focusDistance;
    _lowerLeftCorner = cameraOrigin - (_horizontal / 2) - (_vertical / 2) - (_w * _focusDistance);

    std::clog << "Horizontal vector: " << _horizontal << "\n"
              << "Vertical vector: " << _vertical << "\n"
              << "Lower left corner: " << _lowerLeftCorner << "\n";

    std::clog << "Samples per pixel: " << aaSamples << "\n";

    std::clog << "\nInitialized...\n\n";
}

#endif