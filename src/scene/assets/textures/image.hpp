


#pragma once

#include <cinttypes>

namespace renderer::scene
{
    // gpu resource owner
    class image
    {
    public:
        virtual ~image() = default;
        virtual void create_gpu_resources() = 0;

        uint32_t handler = -1;
    };
}

