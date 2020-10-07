


#pragma once

#include <math/vector.hpp>

namespace math::raytracing
{
    struct ray
    {
        vec3 origin;
        vec3 direction;
    };

    inline math::vec3 ray_at_point(ray r, float point)
    {
        return r.origin + math::normalize(r.direction) * point;
    }
}

