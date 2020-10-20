

#pragma once

#include <shapes/shape.hpp>

namespace shapes
{
    class cylinder : public shape
    {
    public:
        explicit cylinder(float r = 1, float x = 0, float y = 0, float z = 0, float length = 2, float phi_max = M_PI * 2);
        ~cylinder() override = default;

        void generate(
            std::vector<uint8_t>& vertices,
            std::vector<uint8_t>& indices,
            uint32_t smoothness,
            uint64_t cond_bits) override;

    private:
        float m_r;
        float m_x;
        float m_y;
        float m_z;
        float m_length;
        float m_phi_max;
    };
}

