


#pragma once

#include <string>


namespace renderer::scene
{
    struct shader_source
    {
        enum class stage_type
        {
            vertex,
            fragment,
            geometry
        };

        shader_source() = default;
        shader_source(shader_source::stage_type, const std::string&);

        stage_type type{stage_type::vertex};
        std::string code{};
    };
} // namespace renderer::scene
