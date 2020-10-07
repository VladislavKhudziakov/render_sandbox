

#pragma once

#include <material.hpp>


namespace raytracer
{
    class dielectric : public material
    {
    public:
        dielectric(math::vec3 albedo, float ref_c);

        ~dielectric() override = default;
        bool scatter(math::raytracing::ray ray, const hit_record& record, math::vec3& attenuation, math::raytracing::ray& scattered) override;

    private:
        math::vec3 m_albedo;
        float m_refraction_coefficient;

    };
}

