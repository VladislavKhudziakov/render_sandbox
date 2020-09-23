


#pragma once

namespace math::misc
{
    template<typename A, typename B, typename C>
    auto lerp(A a, B b, C c)
    {
        return a * (1. - c) + b * c;
    }
}

