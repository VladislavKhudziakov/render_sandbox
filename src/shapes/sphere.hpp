

#pragma once

#include <shapes/shape.hpp>
#include <math/vector.hpp>
#include <cmath>

namespace shapes
{
    class sphere : public shape
    {
    public:
        explicit sphere(float radius = 1, float x = 0, float y = 0, float z = 0, float psi_max = M_PI * 2);
        ~sphere() override = default;

        void generate(
            std::vector<uint8_t>& vertices,
            std::vector<uint8_t>& indices,
            uint32_t vert_count,
            uint64_t) override;

    private:
        void triangulate(std::vector<math::vec3>& vertices, std::vector<uint8_t>& res);

        float m_radius;
        float m_phi_max;
        float m_x;
        float m_y;
        float m_z;
    };
}
