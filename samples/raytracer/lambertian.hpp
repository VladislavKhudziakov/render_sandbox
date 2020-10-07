

#pragma once

#include <material.hpp>

namespace raytracer
{
    class lambertian : public material
    {
    public:
        lambertian() = default;
        explicit lambertian(math::vec3);
        ~lambertian() override = default;

        bool scatter(
            math::raytracing::ray ray,
            const hit_record& record,
            math::vec3& attenuation,
            math::raytracing::ray& scattered) override;

        math::vec3 albedo{};
    };

}
