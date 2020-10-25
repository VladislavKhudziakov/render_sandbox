

#pragma once

#include <math/matrix.hpp>
#include <memory>

namespace renderer
{
    class renderer;
}

namespace shapes
{
    class shape;
}

namespace renderer::scene
{
    class mesh_instance
    {
    public:
        mesh_instance(renderer*, std::unique_ptr<shapes::shape>);
        ~mesh_instance();

        uint32_t get_handler() const;
        math::mat4 local_transform;
    private:
        renderer* m_renderer;
        std::unique_ptr<shapes::shape> m_shape;
        uint32_t m_handler;
    };
}
