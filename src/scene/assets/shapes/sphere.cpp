

#include "sphere.hpp"

#include <math/misc/misc.hpp>


renderer::scene::shapes::sphere::sphere(
    ::renderer::renderer* r,
    uint32_t smoothness,
    uint32_t cond_bits,
    float radius,
    float phi_max,
    float theta_max)
    : procedural(
        r,
        smoothness,
        cond_bits,
        phi_max / float(M_PI * 2),
        theta_max / float(M_PI))
    , m_radius(radius)
    , m_phi_max(phi_max)
    , m_theta_max(theta_max)
{
}


math::vec3 renderer::scene::shapes::sphere::get_position(float u, float v)
{
    float phi = math::misc::lerp(0, m_phi_max, u);
    float theta = math::misc::lerp(0, m_theta_max, v);
    return {
        sinf(theta) * cosf(phi) * m_radius,
        sinf(theta) * sinf(phi) * m_radius,
        cosf(theta) * m_radius};
}


math::vec3 renderer::scene::shapes::sphere::get_normal(float u, float v, math::vec3 position)
{
    return math::normalize(position);
}


math::vec3 renderer::scene::shapes::sphere::get_tangent(float u, float v, math::vec3 position)
{
    return math::normalize(math::vec3{-m_phi_max * position.y, m_phi_max * position.x, 0});
}
