


#pragma once

namespace renderer::scene
{
    class scene;

    class render_system
    {
    public:
        virtual ~render_system() = default;
        virtual void draw_scene(scene*) = 0;
    };
}

