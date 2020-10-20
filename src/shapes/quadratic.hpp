

#pragma once

#include <shapes/shape.hpp>

namespace shapes
{
    class quadratic : public shape
    {
    public:
        quadratic(float umax = 1, float vmax = 1, clockwise clockwise = clockwise::cw);

        ~quadratic() override = default;
        void generate(std::vector<uint8_t>& vertices, std::vector<uint8_t>& indices, uint32_t smoothness, uint64_t cond_bits) override;

    protected:
        virtual math::vec3 get_position(float u, float v) = 0;
        virtual math::vec3 get_normal(float u, float v, math::vec3 position) = 0;
        virtual math::vec3 get_tangent(float u, float v, math::vec3 position) = 0;

    private:
        float m_umax;
        float m_vmax;
        clockwise m_clockwise;
    };
}

