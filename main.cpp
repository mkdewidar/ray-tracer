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
#include "quad.h"

#include "camera.h"

using namespace std::chrono_literals;

void write_ppm_color(Color const & pixelColor) {
    write_color(std::cout, pixelColor);
}

void post_initialize(Camera const & camera) {
    std::cout << "P3\n" << camera.imageWidth << " " << camera.imageHeight << "\n255\n";
}

void random_spheres() {
    auto world = HittableList();

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

    std::clog << "World contains objects: \n"
              << world
              << "\n" << std::flush;

    Camera camera = Camera();

    camera.cameraOrigin = Point3(7, 2, 6);
    camera.cameraTarget = Point3(0, 0, 0);

    camera.render(std::make_shared<BvhNode>(world), post_initialize, write_ppm_color);
}

void two_spheres() {
    auto world = HittableList();

    // ground
    auto groundTexture = std::make_shared<CheckeredTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    world.add(std::make_shared<Sphere>(Point3(0, -10, 0),
                                       10,
                                       std::make_shared<LambertianMaterial>(groundTexture)));
    world.add(std::make_shared<Sphere>(Point3(0, 10, 0),
                                       10,
                                       std::make_shared<LambertianMaterial>(groundTexture)));

    std::clog << "World contains objects: \n"
              << world
              << "\n" << std::flush;

    Camera camera = Camera();

    camera.cameraOrigin = Point3(7, 2, 6);
    camera.cameraTarget = Point3(0, 0, 0);

    camera.render(std::make_shared<BvhNode>(world), post_initialize, write_ppm_color);
}

void quads() {
    auto world = HittableList();

    world.add(std::make_shared<Quad>(Point3(-3, -2, 5),
                                     Vec3(0, 0,-4),
                                     Vec3(0, 4, 0),
                                     std::make_shared<LambertianMaterial>(Color(1.0, 0.2, 0.2))));
    world.add(std::make_shared<Quad>(Point3(-2, -2, 0),
                                     Vec3(4, 0, 0),
                                     Vec3(0, 4, 0),
                                     std::make_shared<LambertianMaterial>(Color(0.2, 1.0, 0.2))));
    world.add(std::make_shared<Quad>(Point3(3, -2, 1),
                                     Vec3(0, 0, 4),
                                     Vec3(0, 4, 0),
                                     std::make_shared<LambertianMaterial>(Color(0.2, 0.2, 1.0))));
    world.add(std::make_shared<Quad>(Point3(-2, 3, 1),
                                     Vec3(4, 0, 0),
                                     Vec3(0, 0, 4),
                                     std::make_shared<LambertianMaterial>(Color(1.0, 0.5, 0.0))));
    world.add(std::make_shared<Quad>(Point3(-2, -3, 5),
                                     Vec3(4, 0, 0),
                                     Vec3(0, 0, -4),
                                     std::make_shared<LambertianMaterial>(Color(0.2, 0.8, 0.8))));

    std::clog << "World contains objects: \n"
              << world
              << "\n" << std::flush;

    Camera camera = Camera();

    camera.aspectRatio = 1.0;
    camera.imageWidth = 500;
    camera.cameraOrigin = Point3(0, 0, 9);
    camera.cameraTarget = Point3(0, 0, 0);

    camera.render(std::make_shared<BvhNode>(world), post_initialize, write_ppm_color);
}

void simple_lights() {
    auto world = HittableList();

    world.add(std::make_shared<Sphere>(Point3(0,2,0),
                                       2,
                                       std::make_shared<LambertianMaterial>(Color(0.2, 0.2, 1.0))));
    // ground
    auto checkeredTexture = std::make_shared<CheckeredTexture>(0.32, Color(1, 1, 1), Color(0.9, 0.1, 0.9));
    world.add(std::make_shared<Quad>(Point3(-10, 0, -10),
                                     Vec3(0, 0, 20),
                                     Vec3(20, 0, 0),
                                     std::make_shared<LambertianMaterial>(checkeredTexture)));

    // light
    world.add(std::make_shared<Quad>(Point3(3, 2, -2),
                                     Vec3(2, 0, 0),
                                     Vec3(0, 2, 0),
                                     std::make_shared<DiffuseLightMaterial>(Color(4, 4, 4))));
    world.add(std::make_shared<Sphere>(Point3(0, 7, 0),
                                       2,
                                       std::make_shared<DiffuseLightMaterial>(Color(5, 0, 0))));

    std::clog << "World contains objects: \n"
              << world
              << "\n" << std::flush;

    Camera camera = Camera();

    camera.cameraOrigin = Point3(6, 3, 6);
    camera.cameraTarget = Point3(0, 2, 0);
    camera.backgroundColor = Color(0, 0, 0);

    camera.render(std::make_shared<BvhNode>(world), post_initialize, write_ppm_color);
}

void cornell_box() {
    auto world = HittableList();

    auto red   = std::make_shared<LambertianMaterial>(Color(0.65, 0.05, 0.05));
    auto white = std::make_shared<LambertianMaterial>(Color(0.73, 0.73, 0.73));
    auto green = std::make_shared<LambertianMaterial>(Color(0.12, 0.45, 0.15));
    auto light = std::make_shared<DiffuseLightMaterial>(Color(15, 15, 15));

    world.add(std::make_shared<Quad>(Point3(555, 0, 0),
                                     Vec3(0, 555, 0),
                                     Vec3(0, 0, 555),
                                     green));
    world.add(std::make_shared<Quad>(Point3(0, 0, 0),
                                     Vec3(0, 555, 0),
                                     Vec3(0, 0, 555),
                                     red));
    world.add(std::make_shared<Quad>(Point3(343, 554, 332),
                                     Vec3(-130, 0, 0),
                                     Vec3(0, 0, -105),
                                     light));
    world.add(std::make_shared<Quad>(Point3(0, 0, 0),
                                     Vec3(555, 0, 0),
                                     Vec3(0, 0, 555),
                                     white));
    world.add(std::make_shared<Quad>(Point3(555, 555, 555),
                                     Vec3(-555, 0, 0),
                                     Vec3(0, 0, -555),
                                     white));
    world.add(std::make_shared<Quad>(Point3(0, 0, 555),
                                     Vec3(555, 0, 0),
                                     Vec3(0, 555, 0),
                                     white));

    std::clog << "World contains objects: \n"
              << world
              << "\n" << std::flush;

    Camera camera = Camera();

    camera.aspectRatio = 1.0;
    camera.imageWidth = 600;
    camera.fieldOfView = 40;
    camera.cameraOrigin = Point3(278, 278, -800);
    camera.cameraTarget = Point3(278, 278, 0);
    camera.backgroundColor = Color(0, 0, 0);

    camera.render(std::make_shared<BvhNode>(world), post_initialize, write_ppm_color);
}

//         ^ y
//         |
//         |
//        /---> x
//       /
//      z (i.e positive z is out of the screen towards you)

int main() {

    switch (5) {
        case 1: random_spheres(); break;
        case 2: two_spheres(); break;
        case 3: quads(); break;
        case 4: simple_lights(); break;
        case 5: cornell_box(); break;
    }

    return 0;
}
