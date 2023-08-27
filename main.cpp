#include <iostream>
#include <thread>

#include "vec3.h"
#include "color.h"
#include "random.h"

#include "ray.h"

#include "hittable.h"
#include "texture.h"
#include "material.h"
#include "sphere.h"
#include "hittable_list.h"
#include "aabb.h"
#include "bvh_node.h"

#include "camera.h"

using namespace std::chrono_literals;

void write_ppm_color(Color const & pixelColor) {
    write_color(std::cout, pixelColor);
}

void post_initialize(Camera const & camera) {
    std::cout << "P3\n" << camera.imageWidth << " " << camera.imageHeight << "\n255\n";
}

void random_spheres(HittableList & world) {

    // ground
    auto groundTexture = std::make_shared<CheckeredTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    world.add(std::make_shared<Sphere>(Point3(0.0, -1000, 0),
                                       1000,
                                       std::make_shared<LambertianMaterial>(groundTexture)));

    for (int x = -11; x < 11; x++) {
        for (int z = -11; z < 11; z++) {
            auto randomMaterialChoice = random_double();
            auto sphereCenter = Point3(x + (0.9 * random_double()), 0.2, z + (0.9 * random_double()));

            if ((sphereCenter - Point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> randomizedMaterial;

                if (randomMaterialChoice < 0.8) {
                    // diffuse
                    randomizedMaterial = std::make_shared<LambertianMaterial>(Color::random());
                } else if (randomMaterialChoice < 0.95) {
                    // metal
                    randomizedMaterial = std::make_shared<MetalMaterial>(Color::random(0.5, 1), random_double(0, 0.5));
                } else {
                    // glass
                    randomizedMaterial = std::make_shared<DielectricMaterial>(1.5);
                }

                world.add(std::make_shared<Sphere>(sphereCenter,
                                                   sphereCenter + Point3(0, random_double(0, 0.5), 0),
                                                   0.2,
                                                   randomizedMaterial));
            }
        }
    }

    // dielectric bubble
    // two dielectrics inside each other, with the one inside being "inside out"
    world.add(std::make_shared<Sphere>(Point3(-8, 1, 0),
                                       1,
                                       std::make_shared<DielectricMaterial>(1.5)));
    world.add(std::make_shared<Sphere>(Point3(-8, 1, 0),
                                       -0.95,
                                       std::make_shared<DielectricMaterial>(1.5)));

    // diffuse
    world.add(std::make_shared<Sphere>(Point3(-4, 1, 0),
                                       1,
                                       std::make_shared<LambertianMaterial>(Color(0.4, 0.2, 0.1))));

    // dielectric
    world.add(std::make_shared<Sphere>(Point3(0, 1, 0),
                                       1,
                                       std::make_shared<DielectricMaterial>(1.5)));

    // metallic
    world.add(std::make_shared<Sphere>(Vec3(4, 1, 0),
                                       1,
                                       std::make_shared<MetalMaterial>(Color(0.7, 0.6, 0.5), 0)));
}

void two_spheres(HittableList & world) {

    // ground
    auto groundTexture = std::make_shared<CheckeredTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    world.add(std::make_shared<Sphere>(Point3(0, -10, 0),
                                       10,
                                       std::make_shared<LambertianMaterial>(groundTexture)));
    world.add(std::make_shared<Sphere>(Point3(0, 10, 0),
                                       10,
                                       std::make_shared<LambertianMaterial>(groundTexture)));
}

//         ^ y
//         |
//         |
//        /---> x
//       /
//      z (i.e positive z is out of the screen towards you)

int main() {
    auto world = HittableList();

    switch (2) {
        case 1: random_spheres(world); break;
        case 2: two_spheres(world); break;
    }

    std::clog << "World contains objects: \n"
              << world
              << "\n" << std::flush;

    Camera camera = Camera();

    camera.cameraOrigin = Point3(7, 2, 6);
    camera.cameraTarget = Point3(0, 0, 0);

    camera.render(std::make_shared<BvhNode>(world), post_initialize, write_ppm_color);

    return 0;
}
