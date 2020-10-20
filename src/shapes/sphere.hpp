

#pragma once

#include <shapes/shape.hpp>
#include <math/vector.hpp>
#include <cmath>

namespace shapes
{
    class sphere : public shape
    {
    public:
        explicit sphere(float radius = 1, float x = 0, float y = 0, float z = 0, float phi_max = M_PI * 2, float theta_max = M_PI);
        ~sphere() override = default;

        void generate(
            std::vector<uint8_t>& vertices,
            std::vector<uint8_t>& indices,
            uint32_t smoothness,
            uint64_t) override;

    private:
        void triangulate(std::vector<math::vec3>& vertices, std::vector<uint8_t>& res, size_t total);

        float m_radius;
        float m_phi_max;
        float m_theta_max;
        float m_x;
        float m_y;
        float m_z;
    };
}
