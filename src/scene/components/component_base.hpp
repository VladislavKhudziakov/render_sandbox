

#pragma once

#include <cinttypes>

namespace renderer::scene
{
    class component_base
    {
        friend class scene;

    public:
        virtual ~component_base() = default;

    private:
        uint32_t object_handler = -1;
    };
}

