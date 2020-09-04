


#pragma once

#include <renderer/renderer.hpp>

namespace misc
{
    class images_loader
    {
    public:
        renderer::texture_descriptor load_2d_texture(const std::string& file);
    };
} // namespace misc
