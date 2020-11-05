


#pragma once

#include <cinttypes>

namespace renderer
{
    class renderer;
}


namespace renderer::scene
{
    class shader
    {
    public:
        virtual ~shader() = default;
        virtual void create_gpu_resources() = 0;

        uint32_t handler = -1;
    };
}

