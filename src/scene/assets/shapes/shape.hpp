

#pragma once

#include <cinttypes>

namespace renderer
{
    class renderer;
}


namespace renderer::scene::shapes
{
    class shape
    {
    public:
        explicit shape(::renderer::renderer* r)
            : m_renderer(r)
        {
        }

        virtual ~shape() = default;
        virtual void create_gpu_resources() = 0;
        uint32_t handler = -1;

    protected:
        ::renderer::renderer* m_renderer;
    };
} // namespace shapes
