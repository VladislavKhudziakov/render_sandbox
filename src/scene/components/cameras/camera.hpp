


#pragma once

#include <cstddef>

#include <scene/components/component_base.hpp>

namespace renderer::scene
{
    class mesh_instance;
    class render_technique;

    class camera : public component_base
    {
    public:
        explicit camera(render_technique* p)
            : render_technique(p)
        {
        }

        ~camera() override = default;
        virtual void draw_instances(mesh_instance**, size_t instances_count) = 0;
        render_technique* render_technique;
    };
} // namespace renderer::scene
