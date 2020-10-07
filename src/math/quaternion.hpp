

#pragma once

#include <math/vector.hpp>
#include <math/matrix.hpp>


namespace math
{
    struct quaternion
    {
        math::vec3 v;
        float w;
    };

    inline void operator*=(quaternion& a, quaternion b)
    {
        a = {cross(a.v, b.v) + b.w * a.v + a.w * b.v, a.w * b.w - dot(a.v, b.v)};
    }

    template <typename Scalar, typename = std::enable_if<!std::is_same_v<Scalar, quaternion>>>
    void operator*=(quaternion& a, Scalar b)
    {
        a = {a.v * b, a.w * b};
    }

    inline quaternion operator*(quaternion a, quaternion b)
    {
        a *= b;
        return a;
    }

    template <typename Scalar, typename = std::enable_if<!std::is_same_v<Scalar, quaternion>>>
    quaternion operator*(quaternion a, Scalar b)
    {
        a *= b;
        return a;
    }

    template <typename Scalar, typename = std::enable_if<!std::is_same_v<Scalar, quaternion>>>
    quaternion operator*(Scalar b, quaternion a)
    {
        a *= b;
        return a;
    }

    template <typename Scalar, typename = std::enable_if<!std::is_same_v<Scalar, quaternion>>>
    void operator/=(quaternion& a, Scalar b)
    {
        auto inv = 1 / b;
        a = {a.v * inv, a.w * inv};
    }

    template <typename Scalar, typename = std::enable_if<!std::is_same_v<Scalar, quaternion>>>
    quaternion operator/(quaternion a, Scalar b)
    {
        a /= b;
        return a;
    }

    inline void operator+=(quaternion& a, quaternion b)
    {
        a = {a.v + b.v, a.w + b.w};
    }

    inline quaternion operator+(quaternion a, quaternion b)
    {
        a += b;
        return a;
    }

    inline void operator-=(quaternion& a, quaternion b)
    {
        a = {a.v - b.v, a.w - b.w};
    }

    inline quaternion operator-(quaternion a, quaternion b)
    {
        a -= b;
        return a;
    }

    inline float dot(quaternion a, quaternion b)
    {
        return dot(a.v, b.v) + a.w * b.w;
    }

    inline float length(quaternion q)
    {
        return sqrtf(dot(q, q));
    }

    inline quaternion normalize(quaternion q)
    {
        const auto l = length(q);
        return {q.v / l, q.w / l};
    }

    inline quaternion adjacent(quaternion q)
    {
        return {-q.v, q.w};
    }

    inline quaternion inverse(quaternion q)
    {
        auto len_squared = dot(q, q);
        auto adj = adjacent(q);
        return adj / len_squared;
    }

    math::mat4 to_matrix(quaternion q)
    {
        mat4 res;

        res[0][0] = 1 - 2 * (q.v.y * q.v.y + q.v.z * q.v.z);
        res[1][0] = 2 * (q.v.x * q.v.y - q.v.z * q.w);
        res[2][0] = 2 * (q.v.x * q.v.z + q.v.y * q.w);

        res[0][1] = 2 * (q.v.x * q.v.y + q.v.z * q.w);
        res[1][1] = 1 - 2 * (q.v.x * q.v.x + q.v.z * q.v.z);
        res[2][1] = 2 * (q.v.y * q.v.z - q.v.x * q.w);

        res[0][2] = 2 * (q.v.x * q.v.z - q.v.y * q.w);
        res[1][2] = 2 * (q.v.y * q.v.z + q.v.x * q.w);
        res[2][2] = 1 - 2 * (q.v.x * q.v.x + q.v.y * q.v.y);

        return res;
    }

    quaternion slerp(quaternion a, quaternion b, float t)
    {
        const auto c = dot(a, b);
        if (c > .9995f)
            return normalize((1 - t) * a + t * b);
        else {
            auto theta = std::acos(std::clamp(c, -1.f, 1.f));
            auto thetap = theta * t;
            quaternion qperp = normalize(a - b * c);
            return normalize(a * std::cos(thetap) + qperp * std::sin(thetap));
        }
    }

    quaternion quaternion_rotation(math::vec3 v, float angle)
    {
        angle *= 0.5;
        return {sinf(angle) * v, cosf(angle)};
    }
}

