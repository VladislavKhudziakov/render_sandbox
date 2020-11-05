

#pragma once

#include <scene/assets/shapes/procedural.hpp>
#include <math/vector.hpp>
#include <cmath>

namespace renderer::scene::shapes
{
    class sphere : public procedural
    {
    public:
        explicit sphere(
            ::renderer::renderer*,
            uint32_t smoothness,
            uint32_t cond_bits,
            float radius = 1,
            float phi_max = M_PI * 2,
            float theta_max = M_PI);

        ~sphere() override = default;

    protected:
        math::vec3 get_position(float u, float v) override;
        math::vec3 get_normal(float u, float v, math::vec3 position) override;
        math::vec3 get_tangent(float u, float v, math::vec3 position) override;

    private:
        float m_radius;
        float m_phi_max;
        float m_theta_max;
    };
} // namespace renderer::scene::shapes
