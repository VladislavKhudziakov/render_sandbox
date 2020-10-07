


#pragma once

#include <math/raytracing/ray.hpp>
#include <hit_record.hpp>


namespace raytracer
{
    class hit_detector
    {
    public:
        virtual ~hit_detector() = default;
        virtual bool hit(math::raytracing::ray, hit_record&, float t_min = 0, float t_max = std::numeric_limits<float>::max()) = 0;
    };
}

