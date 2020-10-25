

#pragma once

#include <math/vector.hpp>

#include <vector>
#include <cinttypes>

namespace shapes
{
    class shape
    {
    public:
        virtual ~shape() = default;
        virtual void generate(
            std::vector<uint8_t>& vertices,
            std::vector<uint8_t>& indices) = 0;
    };
} // namespace shapes
