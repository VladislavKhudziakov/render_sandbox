

#pragma once

#include <material.hpp>

namespace raytracer
{
    class metal : public material
    {
    public:
        metal() = default;
        explicit metal(math::vec3, float metalness = 1.);
        ~metal() override = default;
        bool scatter(math::raytracing::ray ray, const hit_record& record, math::vec3& attenuation, math::raytracing::ray& scattered) override;

    private:
        math::vec3 m_albedo{};
        float m_metalness;
    };
}

