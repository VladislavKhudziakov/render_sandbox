

#include "cylinder.hpp"

#include <math/misc/misc.hpp>

shapes::cylinder::cylinder(float r, float x, float y, float z, float length, float phi_max)
    : m_r(r)
    , m_x(x)
    , m_y(y)
    , m_z(z)
    , m_length(length)
    , m_phi_max(phi_max)
{
}


void shapes::cylinder::generate(
    std::vector<uint8_t>& vertices,
    std::vector<uint8_t>& indices,
    uint32_t smoothness,
    uint64_t cond_bits)
{
    std::vector<math::vec3> res_vertices;
    res_vertices.reserve(smoothness * smoothness);
    for (size_t x = 0; x < smoothness + 1; ++x) {
        for (size_t y = 0; y < smoothness + 1; ++y) {
            float u = std::clamp(float(x) / float(smoothness), 0.0f, 1.0f);
            float v = std::clamp(float(y) / float(smoothness), 0.0f, 1.0f);

            float angle = math::misc::lerp(0, m_phi_max, u);

            math::vec3 pos = {
                m_x + m_r * cosf(angle),
                m_y + m_r * sinf(angle),
                math::misc::lerp(m_z - m_length * 0.5f, m_z + m_length * 0.5f, v)};

            res_vertices.emplace_back(pos);
        }
    }
}
