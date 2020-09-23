#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <math/vector.hpp>
#include <math/misc/misc.hpp>
#include <math/raytracing/ray.hpp>
#include <iostream>


constexpr static size_t width = 800;
constexpr static size_t height = 600;

math::ubvec3 image[height][width];


struct sphere
{
    math::vec3 origin;
    float radius;

    bool hit(math::raytracing::ray ray, float& t) const
    {
        math::vec3 oc = ray.origin - origin;
        float a = math::dot(ray.direction, ray.direction);
        float b = 2.f * math::dot(oc, ray.direction);
        float c = math::dot(oc, oc) - radius * radius;
        float d = b * b - 4.f * a * c;

        if (d >= 0.f) {
            t = (-b - sqrt(d)) / (2.f * a);
        }

        return d >= 0.f && t > 0;
    }
};

int main()
{
    auto color = [](math::vec3 direction)
    {
        direction = math::normalize(direction) * 0.5 + 0.5;
        return math::misc::lerp(math::vec3{1, 1, 1}, math::vec3{0.5, 0.7, 1.0}, direction);
    };

    math::vec3 camera_width = {4, 0, 0};
    math::vec3 camera_height = {0, 3, 0};
    math::vec3 left_bottom = {-2, -1.5, -1};
    math::vec3 origin = {0, 0, 0};

    sphere spheres[] = {
        {{0, 0, -1}, 0.5},
        {{0, -100.5, -1}, 100}
    };

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            auto dx = camera_width * (float(x) / width);
            auto dy = camera_height * (float(y) / height);
            auto dir = math::normalize(left_bottom + dx + dy);

            math::raytracing::ray ray{origin, dir};
            float t;

            bool sphere_hit_success = false;

            for (auto& s : spheres) {
                if (s.hit(ray, t)) {
                    auto n = math::normalize((ray.origin + t * ray.direction) - s.origin);
                    n = n * 0.5 + 0.5;
                    image[(height - 1) - y][x].x = n.x * 255.f;
                    image[(height - 1) - y][x].y = n.y * 255.f;
                    image[(height - 1) - y][x].z = n.z * 255.f;
                    sphere_hit_success = true;
                    break;
                }
            }

            if (!sphere_hit_success) {
                auto c = color(ray.direction);

                image[(height - 1) - y][x].x = c.x * 255.99f;
                image[(height - 1) - y][x].y = c.y * 255.99f;
                image[(height - 1) - y][x].z = c.z * 255.99f;
            }
        }
    }

    stbi_write_png("result.png", width, height, 3, image, 0);
    return 0;
}