

#pragma once

#include <math/raytracing/ray.hpp>
#include <hit_record.hpp>

namespace raytracer
{
    class material
    {
    public:
        virtual ~material() = default;

        virtual bool scatter(
            math::raytracing::ray,
            const raytracer::hit_record&,
            math::vec3& attenuation,
            math::raytracing::ray& scattered) = 0;
    };
}

