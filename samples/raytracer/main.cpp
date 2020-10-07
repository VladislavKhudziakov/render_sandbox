#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "hit_detector.hpp"
#include "sphere.hpp"
#include "hit_detectors_list.hpp"
#include "camera.hpp"
#include "lambertian.hpp"
#include "metal.hpp"
#include "dielectric.hpp"

#include <math/vector.hpp>
#include <math/misc/misc.hpp>
#include <math/raytracing/ray.hpp>

#include <vector>
#include <iostream>

#include <future>

constexpr static size_t width = 800;
constexpr static size_t height = 600;
constexpr static size_t samples_count = 4;

math::ubvec3 image[height][width];

math::vec3 color(math::raytracing::ray ray, raytracer::hit_detector* world, size_t recursion_depth = 0)
{
    raytracer::hit_record r{};
    if (recursion_depth <= 500 && world->hit(ray, r, 0.0001) && r.material != nullptr) {
        math::raytracing::ray new_ray{};
        math::vec3 attenuation;
        r.material->scatter(ray, r, attenuation, new_ray);
        return attenuation * color(new_ray, world, ++recursion_depth);
    } else {
        auto c = ray.direction * 0.5 + 0.5;
        return math::misc::lerp(math::vec3{1, 1, 1}, math::vec3{0.5, 0.6, 0.7}, c);
    }
}

int main()
{
    raytracer::hit_detectors_list l;
    l.add_detector<raytracer::sphere>(math::vec3{-1., 0, -1}, 0.5, std::make_unique<raytracer::metal>(math::vec3 {0.5, 0.3, 0.4}));
    l.add_detector<raytracer::sphere>(math::vec3{1., 0, -1}, 0.5, std::make_unique<raytracer::dielectric>(math::vec3{1.f, 1.f, 1.f}, 1.5f));
    l.add_detector<raytracer::sphere>(math::vec3{0, 0, -1}, 0.5, std::make_unique<raytracer::lambertian>(math::vec3 {0.7, 0.8, 0}));
    l.add_detector<raytracer::sphere>(math::vec3{0, -100.5, -1}, 100, std::make_unique<raytracer::lambertian>(math::vec3 {0.2, 0.7, 0.2}));

   raytracer::camera c{M_PI_2, 4, 3, {0, 0, 1.}, {0., 0., -1.}};

    auto threads_count = std::thread::hardware_concurrency();
    auto chunk_width = width / threads_count;
    int chunk_alignment = width % threads_count;
    auto curr_offset = 0;

    std::vector<std::future<void>> futures;
    futures.reserve(threads_count);

    for (int i = 0; i < threads_count; ++i, chunk_alignment--) {
        auto bias = chunk_alignment > 0 ? 1 : 0;
        auto x_max = curr_offset + chunk_width + bias;

        auto f = std::async([x_max, curr_offset, &c, &l]() {
            for (int x = curr_offset; x < x_max; ++x) {
                for (int y = 0; y < height; ++y) {
                    math::vec3 curr_color{};

                    for (int i = 0; i < samples_count; ++i) {
                        const auto ray = c.gen_ray((float(x) + math::misc::rand_float()) / width, (float(y) + math::misc::rand_float()) / height);
                        curr_color += ::color(ray, &l);
                    }

                    curr_color /= samples_count;

                    image[(height - 1) - y][x].x = sqrt(curr_color.x) * 255.99f;
                    image[(height - 1) - y][x].y = sqrt(curr_color.y) * 255.99f;
                    image[(height - 1) - y][x].z = sqrt(curr_color.z) * 255.99f;
                }
            }
        });

        futures.emplace_back(std::move(f));
        curr_offset += chunk_width + bias;
    }

    while (!futures.empty()) {
        futures.back().get();
        futures.pop_back();
    }


    stbi_write_png("result.png", width, height, 3, image, 0);
    return 0;
}