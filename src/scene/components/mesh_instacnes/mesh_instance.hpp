

#pragma once

#include <scene/components/component_base.hpp>

#include <math/matrix.hpp>
#include <memory>


namespace renderer
{
    class renderer;
}

namespace renderer::scene::shapes
{
    class shape;
}

namespace renderer::scene
{
    class mesh_instance : public component_base
    {
    public:
        mesh_instance() = default;
        explicit mesh_instance(::renderer::scene::shapes::shape*);
        ~mesh_instance() override = default;
        shapes::shape* shape{nullptr};
    };
} // namespace renderer::scene
