


#pragma once

#include <cstdlib>
#include <math/vector.hpp>

namespace math::misc
{
    template<typename A, typename B, typename C>
    auto lerp(A a, B b, C c)
    {
        return a * (1. - c) + b * c;
    }

    template<typename A, typename B>
    auto reflect(A v, B n)
    {
        return  v - 2. * math::dot(v, n) * n;
    }


    inline bool refract(math::vec3 v, math::vec3 n, math::vec3& res, float ni_over_nt)
    {
        v = math::normalize(v);
        float dt = math::dot(v, n);
        float d = 1.f - ni_over_nt * ni_over_nt * (1.f - dt * dt);

        if (d < 0.f) {
            return false;
        }

        res = ni_over_nt * (v - n * dt) - n * sqrt(d);
        return true;
    }

    inline float rand_float()
    {
        return float(std::rand()) / RAND_MAX;
    }

    inline math::vec3 random_in_unit_sphere()
    {
        auto x = rand_float() * 2.0f - 1.0f;
        auto y = rand_float() * 2.0f - 1.0f;
        auto z = rand_float() * 2.0f - 1.0f;

        return math::normalize(math::vec3{x, y, z});
    }

    inline math::vec3 random_in_unit_disc()
    {
        auto x = rand_float() * 2.0f - 1.0f;
        auto y = rand_float() * 2.0f - 1.0f;

        return math::normalize(math::vec3{x, y, 0.0});
    }
}

