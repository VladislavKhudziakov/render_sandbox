


#pragma once

#include <scene/components/cameras/camera.hpp>

namespace renderer::scene
{
    class default_camera : public camera
    {
    public:
        explicit default_camera(class render_technique*);
        ~default_camera() override = default;
        void draw_instances(mesh_instance** p_instance, size_t instances_count) override;
    };
} // namespace renderer::scene
