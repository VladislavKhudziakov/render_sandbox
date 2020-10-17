

#include "sphere.hpp"

#include <math/misc/misc.hpp>

namespace
{
    template<typename IndexType>
    void generate_indices(std::vector<math::vec3>& vertices, std::vector<uint8_t>& res)
    {
        IndexType total = sqrt(vertices.size());

        std::vector<IndexType> indices;
        indices.reserve(vertices.size() * 1.5f);

        auto to_flat_index = [](IndexType x, IndexType y, IndexType total) {
            return x * total + y;
        };

        for (IndexType x = 0; x < total - 1; x++) {
            for (IndexType y = 0; y < total - 1; ++y) {
                //ccw
                indices.emplace_back(to_flat_index(x, y + 1, total));
                indices.emplace_back(to_flat_index(x + 1, y, total));
                indices.emplace_back(to_flat_index(x, y, total));

                indices.emplace_back(to_flat_index(x, y + 1, total));
                indices.emplace_back(to_flat_index(x + 1, y + 1, total));
                indices.emplace_back(to_flat_index(x + 1, y, total));
            }
        }

        for (IndexType y = 0; y < total - 1; ++y) {
            //ccw
            indices.emplace_back(to_flat_index(0, y + 1, total));
            indices.emplace_back(to_flat_index(total - 1, y, total));
            indices.emplace_back(to_flat_index(0, y, total));

            indices.emplace_back(to_flat_index(0, y + 1, total));
            indices.emplace_back(to_flat_index(total - 1, y + 1, total));
            indices.emplace_back(to_flat_index(total - 1, y, total));
        }

        res.resize(indices.size() * sizeof(indices.front()));
        std::memcpy(res.data(), indices.data(), indices.size() * sizeof(indices.front()));
    }
} // namespace


shapes::sphere::sphere(float radius, float x, float y, float z, float phi_max)
    : m_radius(radius)
    , m_phi_max(phi_max)
    , m_x(x)
    , m_y(y)
    , m_z(z)
{
}


void shapes::sphere::generate(
    std::vector<uint8_t>& res_vertices,
    std::vector<uint8_t>& indices,
    uint32_t vert_count,
    uint64_t cond_bits)
{
    double total = sqrt(double(vert_count));
    double lower = floor(total);
    double diff = 1.0f + (total - lower) / lower;

    std::vector<math::vec3> vertices;
    vertices.reserve(vert_count * 3);

    for (double x = 0; x < total; x += diff) {
        for (double y = 0; y < total; y += diff) {
            float u = std::clamp(float(x) / float(total), 0.0f, 1.0f);
            float v = std::clamp(float(y) / float(total), 0.0f, 1.0f);

            float phi = math::misc::lerp(0, m_phi_max, u);
            float theta = math::misc::lerp(0, M_PI, v);
            vertices.emplace_back(math::vec3{
                m_x + sinf(theta) * cosf(phi) * m_radius,
                m_y + sinf(theta) * sinf(phi) * m_radius,
                m_z + cosf(theta) * m_radius});
        }
    }

    res_vertices.resize(vertices.size() * sizeof(vertices.front()));
    std::memcpy(res_vertices.data(), vertices.data(), vertices.size() * sizeof(vertices.front()));

    if (cond_bits & shape::triangulate) {
        triangulate(vertices, indices);
    }
}


void shapes::sphere::triangulate(std::vector<math::vec3>& vertices, std::vector<uint8_t>& res)
{
    if (vertices.size() * 1.5f <= std::numeric_limits<uint16_t>::max()) {
        generate_indices<uint16_t>(vertices, res);
    } else {
        generate_indices<uint32_t>(vertices, res);
    }
}
