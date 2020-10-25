

#include "cylinder.hpp"

#include <math/misc/misc.hpp>


shapes::cylinder::cylinder(uint32_t smoothness, uint32_t cond_bits, float r, float zmin, float zmax, float phi_max)
    : procedural(smoothness, cond_bits, phi_max / float(M_PI * 2), 1, clockwise::ccw)
    , m_r(r)
    , m_zmin(zmin)
    , m_zmax(zmax)
    , m_phi_max(phi_max)
{
}


math::vec3 shapes::cylinder::get_position(float u, float v)
{
    float phi = math::misc::lerp(0, m_phi_max, u);
    return {
        m_r * cosf(phi),
        m_r * sinf(phi),
        m_zmin + math::misc::lerp(0.0f, m_zmax - m_zmin, v)};
}


math::vec3 shapes::cylinder::get_normal(float u, float v, math::vec3 position)
{
    return math::normalize(math::vec3{position.x, position.y, 0});
}


math::vec3 shapes::cylinder::get_tangent(float u, float v, math::vec3 position)
{
    return math::normalize(math::vec3{-m_phi_max * position.y, m_phi_max * position.x, 0});
}
