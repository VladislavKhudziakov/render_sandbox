

#pragma once

#include <scene/assets/shapes/procedural.hpp>

namespace renderer::scene::shapes
{
    class cylinder : public procedural
    {
    public:
        explicit cylinder(
            ::renderer::renderer* renderer,
            uint32_t smoothness,
            uint32_t cond_bits,
            float r = 1,
            float zmin = -1,
            float zmax = 1,
            float phi_max = M_PI * 2);

        ~cylinder() override = default;

    protected:
        math::vec3 get_position(float u, float v) override;
        math::vec3 get_normal(float u, float v, math::vec3 position) override;
        math::vec3 get_tangent(float u, float v, math::vec3 position) override;


    private:
        float m_r;
        float m_zmin;
        float m_zmax;
        float m_phi_max;
    };
} // namespace renderer::scene::shapes
