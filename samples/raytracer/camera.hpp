


#pragma once

#include <math/vector.hpp>
#include <math/misc/misc.hpp>
#include <math/raytracing/ray.hpp>


namespace raytracer
{
    class hit_detector;

    class camera
    {
    public:
        camera(float fov, float width, float height, math::vec3 o, math::vec3 target, math::vec3 up = {0., 1., 0.});

        math::raytracing::ray3 gen_ray(float x, float y);

    private:
        math::vec3 m_hor;
        math::vec3 m_vert;
        math::vec3 m_origin;
        math::vec3 m_left_bottom;
    };
}
