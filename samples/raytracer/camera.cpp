

#include "camera.hpp"



raytracer::camera::camera(float fov, float width, float height, math::vec3 o, math::vec3 target, math::vec3 up)
{
    float h = tanf(fov * 0.5f);
    float w = h * width / height;
    m_origin = o;
    m_hor = {2 * w, 0., 0.};
    m_vert = {0., 2 * h, 0.};
    m_left_bottom = {-h, -w, -1.};

}


math::raytracing::ray raytracer::camera::gen_ray(float x, float y)
{
    auto dir = math::normalize(m_left_bottom + m_hor * x + m_vert * y);
    return {m_origin, dir};
}

