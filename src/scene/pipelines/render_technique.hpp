

#pragma once

#include <cstddef>

namespace renderer::scene
{
    // configure passes, access to passes attachments trough frame

    class mesh_instance;
    class frame;

    class render_technique
    {
    public:
        virtual ~render_technique() = default;
        virtual void draw_instances(mesh_instance**, size_t instances_count) = 0;
        virtual void get_frames(frame**, size_t frames_count) = 0;
        virtual void get_frames(const frame**, size_t frames_count) const = 0;
    };
}

