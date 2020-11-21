

#pragma once

#include <math/vector.hpp>
#include <vector>


namespace math
{
    struct ray
    {
        vec3 from;
        vec3 to;
    };

    inline bool hit_with_surface(ray ray, math::vec3 vertex, math::vec3 normal, math::vec3& hit_point)
    {
        vec3 ray_dir = ray.to - ray.from;
        auto ray_line_dist = dot(ray_dir, normal);

        if (std::abs(ray_line_dist) < 0.00001) {
            return false;
        }

        vec3 l1 = vertex - ray.from;
        float dist_to_plane = dot(l1, normal);
        float percentage = dist_to_plane / ray_line_dist;

        if (percentage < 0 || percentage > 1) {
            return false;
        }

        hit_point = ray.from + ray_dir * percentage;
        return true;
    }

    inline bool is_point_inside_polygon(math::vec3 point, math::vec3* polygon_vertices, size_t polygon_size)
    {
        std::vector<math::vec3> point_to_vertex_vectors;
        point_to_vertex_vectors.reserve(polygon_size);

        for (int i = 0; i < polygon_size; ++i) {
            point_to_vertex_vectors.emplace_back(normalize(point - polygon_vertices[i]));
        }

        float total = 0;

        for (int i = 0; i < polygon_size - 1; ++i) {
            total += acosf(dot(point_to_vertex_vectors[i + 1], point_to_vertex_vectors[i]));
        }

        total += acosf(dot(point_to_vertex_vectors[polygon_size - 1], point_to_vertex_vectors[0]));

        return std::abs(2.0 * M_PI - total) < 0.000001;
    }
} // namespace math
