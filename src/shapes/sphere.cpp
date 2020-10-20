

#include "sphere.hpp"

#include <math/misc/misc.hpp>

namespace
{
    template<typename IndexType>
    void generate_indices(std::vector<math::vec3>& vertices, std::vector<uint8_t>& res, size_t total)
    {
        std::vector<IndexType> indices;
        indices.reserve(vertices.size() * 1.5f);

        auto to_flat_index = [](IndexType x, IndexType y, IndexType total) {
            return x * total + y;
        };

        for (size_t x = 0; x < total - 1; x++) {
            for (size_t y = 0; y < total - 1; ++y) {
                //ccw
                indices.emplace_back(to_flat_index(x, y + 1, total));
                indices.emplace_back(to_flat_index(x, y, total));
                indices.emplace_back(to_flat_index(x + 1, y, total));

                indices.emplace_back(to_flat_index(x, y + 1, total));
                indices.emplace_back(to_flat_index(x + 1, y, total));
                indices.emplace_back(to_flat_index(x + 1, y + 1, total));
            }
        }

        for (size_t y = 0; y < total - 1; ++y) {
            //ccw
            indices.emplace_back(to_flat_index(total - 1, y + 1, total));
            indices.emplace_back(to_flat_index(total - 1, y, total));
            indices.emplace_back(to_flat_index(0, y, total));

            indices.emplace_back(to_flat_index(total - 1, y + 1, total));
            indices.emplace_back(to_flat_index(0, y + 1, total));
            indices.emplace_back(to_flat_index(0, y, total));
        }

        res.resize(indices.size() * sizeof(indices.front()));
        std::memcpy(res.data(), indices.data(), indices.size() * sizeof(indices.front()));
    }
} // namespace


shapes::sphere::sphere(float radius, float x, float y, float z, float phi_max, float theta_max)
    : m_radius(radius)
    , m_phi_max(phi_max)
    , m_theta_max(theta_max)
    , m_x(x)
    , m_y(y)
    , m_z(z)
{
}


void shapes::sphere::generate(
    std::vector<uint8_t>& res_vertices,
    std::vector<uint8_t>& indices,
    uint32_t smoothness,
    uint64_t cond_bits)
{
    std::vector<math::vec3> vertices;
    std::vector<math::vec2> uvs;
    std::vector<math::vec3> normals;
    std::vector<math::vec3> tangents;

    vertices.reserve(smoothness * smoothness);

    if (cond_bits & shape::gen_uv) {
        uvs.reserve(smoothness * smoothness);
    }

    if (cond_bits & shape::gen_normal) {
        normals.reserve(smoothness * smoothness);
    }

    if (cond_bits & shape::gen_tangents) {
        tangents.reserve(smoothness * smoothness);
    }

    for (size_t x = 0; x < smoothness + 1; x++) {
        for (size_t y = 0; y < smoothness + 1; y++) {
            float u = std::clamp(float(x) / float(smoothness), 0.0f, 1.0f);
            float v = std::clamp(float(y) / float(smoothness), 0.0f, 1.0f);

            float phi = math::misc::lerp(0, m_phi_max, u);
            float theta = math::misc::lerp(0, m_theta_max, v);
            vertices.emplace_back(math::vec3{
                m_x + sinf(theta) * cosf(phi) * m_radius,
                m_y + sinf(theta) * sinf(phi) * m_radius,
                m_z + cosf(theta) * m_radius});

            if (cond_bits & shape::gen_uv) {
                uvs.emplace_back(math::vec2{u, v});
            }

            if (cond_bits & shape::gen_normal) {
                normals.emplace_back(math::normalize(vertices.back() - math::vec3{m_x, m_y, m_z}));
            }

            if (cond_bits & shape::gen_tangents) {
                auto tangent = math::normalize(math::vec3 {-m_phi_max * vertices.back().y, m_phi_max * vertices.back().x, 0});
                tangents.emplace_back(tangent);
            }
        }
    }

    auto buffer_size = vertices.size() * sizeof(vertices.front())
                       + uvs.size() * sizeof(uvs.front())
                       + normals.size() * sizeof(normals.front())
                       + tangents.size() * sizeof(tangents.front());

    auto vert_size = sizeof(vertices.front())
                     + (uvs.empty() ? 0 : sizeof(uvs.front()))
                     + (normals.empty() ? 0 : sizeof(normals.front()))
                     + (tangents.empty() ? 0 : sizeof(tangents.front()));


    res_vertices.resize(buffer_size);
    auto begin = res_vertices.data();

    for (int i = 0; i < vertices.size(); ++i, begin += vert_size) {
        std::memcpy(begin, &vertices[i], sizeof(vertices[i]));
        size_t offset = sizeof(vertices[i]);

        if (!uvs.empty()) {
            std::memcpy(begin + offset, &uvs[i], sizeof(uvs[i]));
            offset += sizeof(uvs[i]);
        }

        if (!normals.empty()) {
            std::memcpy(begin + offset, &normals[i], sizeof(normals[i]));
            offset += sizeof(normals[i]);
        }

        if (!tangents.empty()) {
            std::memcpy(begin + offset, &tangents[i], sizeof(tangents[i]));
            offset += sizeof(tangents[i]);
        }
    }

    if (cond_bits & shape::triangulate) {
        triangulate(vertices, indices, smoothness + 1);
    }
}


void shapes::sphere::triangulate(std::vector<math::vec3>& vertices, std::vector<uint8_t>& res, size_t total)
{
    if (vertices.size() <= std::numeric_limits<uint16_t>::max()) {
        generate_indices<uint16_t>(vertices, res, total);
    } else {
        generate_indices<uint32_t>(vertices, res, total);
    }
}
