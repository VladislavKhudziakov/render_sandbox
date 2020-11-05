

#pragma once

#include <scene/assets/shapes/procedural.hpp>

#include <array>

namespace renderer::scene::shapes
{
    class curve : public procedural
    {
    public:
        explicit curve(
            ::renderer::renderer* renderer,
            uint32_t smoothness,
            uint32_t cond_bits,
            math::vec3 p0 = {0, 0, -1},
            math::vec3 p1 = {0, 1, -1.f / 3.f},
            math::vec3 p2 = {0, -1, 1.f / 3.f},
            math::vec3 p3 = {0, 0, 1},
            float width = 2);

        ~curve() override = default;

    protected:
        math::vec3 get_position(float u, float v) override;
        math::vec3 get_normal(float u, float v, math::vec3 position) override;
        math::vec3 get_tangent(float u, float v, math::vec3 position) override;

    private:
        std::array<math::vec3, 4> m_points;
        float m_width;
    };
} // namespace renderer::scene::shapes
