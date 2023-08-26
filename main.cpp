#include <iostream>
#include <thread>


#include "vec3.h"
#include "color.h"
#include "random.h"

#include "ray.h"

#include "hittable.h"
#include "material.h"
#include "sphere.h"

#include "camera.h"

using namespace std::chrono_literals;

void write_ppm_color(Color const & pixelColor) {
    write_color(std::cout, pixelColor);
}

void post_initialize(Camera const & camera) {
    std::cout << "P3\n" << camera.imageWidth << " " << camera.imageHeight << "\n255\n";
}

//         ^ y
//         |
//         |
//        /---> x
//       /
//      z (i.e positive z is out of the screen towards you)

int main() {
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

    Camera camera = Camera();
    camera.render(objects, post_initialize, write_ppm_color);

    return 0;
}
