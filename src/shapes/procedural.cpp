

#include "procedural.hpp"


namespace
{
    template<typename IndexType>
    void generate_indices(
        std::vector<math::vec3>& vertices,
        std::vector<uint8_t>& res, size_t total,
        shapes::clockwise clockwise,
        bool closed = true)
    {
        std::vector<IndexType> indices;
        indices.reserve(vertices.size() * 1.5f);

        auto to_flat_index = [](IndexType x, IndexType y, IndexType total) {
            return x * total + y;
        };

        for (size_t x = 0; x < total - 1; x++) {
            for (size_t y = 0; y < total - 1; ++y) {
                if (clockwise == shapes::clockwise::cw) {
                    indices.emplace_back(to_flat_index(x, y + 1, total));
                    indices.emplace_back(to_flat_index(x, y, total));
                    indices.emplace_back(to_flat_index(x + 1, y, total));

                    indices.emplace_back(to_flat_index(x, y + 1, total));
                    indices.emplace_back(to_flat_index(x + 1, y, total));
                    indices.emplace_back(to_flat_index(x + 1, y + 1, total));
                } else {
                    indices.emplace_back(to_flat_index(x, y + 1, total));
                    indices.emplace_back(to_flat_index(x + 1, y, total));
                    indices.emplace_back(to_flat_index(x, y, total));

                    indices.emplace_back(to_flat_index(x, y + 1, total));
                    indices.emplace_back(to_flat_index(x + 1, y + 1, total));
                    indices.emplace_back(to_flat_index(x + 1, y, total));
                }
            }
        }

        if (closed) {
            for (size_t y = 0; y < total - 1; ++y) {
                if (clockwise == shapes::clockwise::cw) {
                    indices.emplace_back(to_flat_index(total - 1, y + 1, total));
                    indices.emplace_back(to_flat_index(total - 1, y, total));
                    indices.emplace_back(to_flat_index(0, y, total));

                    indices.emplace_back(to_flat_index(total - 1, y + 1, total));
                    indices.emplace_back(to_flat_index(0, y + 1, total));
                    indices.emplace_back(to_flat_index(0, y, total));
                } else {
                    indices.emplace_back(to_flat_index(total - 1, y + 1, total));
                    indices.emplace_back(to_flat_index(0, y, total));
                    indices.emplace_back(to_flat_index(total - 1, y, total));

                    indices.emplace_back(to_flat_index(total - 1, y + 1, total));
                    indices.emplace_back(to_flat_index(0, y, total));
                    indices.emplace_back(to_flat_index(0, y + 1, total));
                }
            }
        }

        res.resize(indices.size() * sizeof(indices.front()));
        std::memcpy(res.data(), indices.data(), indices.size() * sizeof(indices.front()));
    }
} // namespace

shapes::procedural::procedural(uint32_t smoothness, uint32_t cond_bits, float umax, float vmax, clockwise clockwise , bool closed)
    : m_smoothness(smoothness)
    , m_cond_bits(cond_bits)
    , m_umax(umax)
    , m_vmax(vmax)
    , m_clockwise(clockwise)
    , m_closed(closed)
{
}


void shapes::procedural::generate(
    std::vector<uint8_t>& vert_buf,
    std::vector<uint8_t>& ind_buf)
{
    std::vector<math::vec3> vertices;
    std::vector<math::vec2> uvs;
    std::vector<math::vec3> normals;
    std::vector<math::vec3> tangents;

    vertices.reserve(m_smoothness * m_smoothness);

    if (m_cond_bits & gen_uv) {
        uvs.reserve(m_smoothness * m_smoothness);
    }

    if (m_cond_bits & gen_normal) {
        normals.reserve(m_smoothness * m_smoothness);
    }

    if (m_cond_bits & gen_tangents) {
        tangents.reserve(m_smoothness * m_smoothness);
    }

    for (size_t x = 0; x < m_smoothness + 1; x++) {
        float u = std::clamp(float(x) / float(m_smoothness), 0.0f, 1.0f);
        if (u > m_umax) {
            continue;
        }

        for (size_t y = 0; y < m_smoothness + 1; y++) {
            float v = std::clamp(float(y) / float(m_smoothness), 0.0f, 1.0f);

            if (v > m_umax) {
                continue;
            }

            auto pos = get_position(u, v);

            vertices.emplace_back(pos);

            if (m_cond_bits & gen_uv) {
                uvs.emplace_back(math::vec2{u, v});
            }

            if (m_cond_bits & gen_normal) {
                normals.emplace_back(get_normal(u, v, pos));
            }

            if (m_cond_bits & gen_tangents) {
                tangents.emplace_back(get_tangent(u, v, pos));
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


    vert_buf.resize(buffer_size);
    auto begin = vert_buf.data();

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

    if (m_cond_bits & triangulate) {
        if (vertices.size() <= std::numeric_limits<uint16_t>::max()) {
            generate_indices<uint16_t>(vertices, ind_buf, m_smoothness + 1, m_clockwise, m_closed);
        } else {
            generate_indices<uint32_t>(vertices, ind_buf, m_smoothness + 1, m_clockwise, m_closed);
        }
    }
}
