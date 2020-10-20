

#pragma once

#include <math/vector.hpp>

#include <vector>
#include <cinttypes>

namespace shapes
{
    class shape
    {
    public:
        constexpr static size_t gen_uv = 1ull << 0;
        constexpr static size_t gen_normal = 1ull << 1;
        constexpr static size_t gen_tangents = 1ull << 2;
        constexpr static size_t triangulate = 1ull << 3;

        virtual ~shape() = default;

        virtual void generate(
            std::vector<uint8_t>& vertices,
            std::vector<uint8_t>& indices,
            uint32_t smoothness,
            uint64_t cond_bits) = 0;
    };
}

