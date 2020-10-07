


#pragma once

#include <math/vector.hpp>

namespace raytracer
{
    class material;

    struct hit_record
    {
        math::vec3 point;
        math::vec3 normal;
        float t;
        material* material;
    };

}
