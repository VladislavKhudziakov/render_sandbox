

#include "curve.hpp"

#include <math/misc/misc.hpp>

shapes::curve::curve(
    math::vec3 p0,
    math::vec3 p1,
    math::vec3 p2,
    math::vec3 p3,
    float width)
    : quadratic(1, 1, clockwise::cw, false)
    , m_points{p0, p1, p2, p3}
    , m_width(width)
{
}


math::vec3 shapes::curve::get_position(float u, float v)
{
    float x = math::misc::lerp(-m_width * 0.5, m_width * 0.5, u);

    float one_minus_v = 1 - v;
    float one_minus_v_pow3 = one_minus_v * one_minus_v * one_minus_v;
    float one_minus_v_pow2 = one_minus_v * one_minus_v;

    auto res = one_minus_v_pow3 * m_points[0]
               + 3 * one_minus_v_pow2 * v * m_points[1]
               + 3 * one_minus_v * (v * v) * m_points[2]
               + (v * v * v) * m_points[3];

    res.x = x;

    return res;
}


math::vec3 shapes::curve::get_normal(float u, float v, math::vec3 position)
{
    auto dfdv = get_tangent(u, v, position);
    auto dfdu = math::vec3{-1, 0, 0};

    return math::cross(dfdu, dfdv);
}


math::vec3 shapes::curve::get_tangent(float u, float v, math::vec3 position)
{
    float one_minus_v = 1 - v;
    float one_minus_v_pow2 = one_minus_v * one_minus_v;
    float v_pow2 = v * v;

    auto a = m_points[0];
    auto b = m_points[1];
    auto c = m_points[2];
    auto d = m_points[3];

    auto res = -3 * (a * one_minus_v_pow2 + b * (-3 * v_pow2 + 4 * v - 1) + v * (3 * c * v - 2 * c - d * v));

    res.x = 0;

    return math::normalize(res);
}
