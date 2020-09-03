


#pragma once

#include <cinttypes>
#include <vector>
#include <string>

namespace renderer
{
    enum class type
    {
        f32, f16, u32, u16, u8
    };

    enum class shader_stage_name
    {
        vertex, fragment
    };

    struct vertex_attribute
    {
        type type;
        uint32_t elements_count;
    };

    struct mesh_layout_descriptor
    {
        std::vector<vertex_attribute> vertex_attributes;
        std::vector<uint8_t> vertex_data;
        std::vector<uint8_t> index_data;
        type index_type = type::u16;
    };

    struct shader_stage
    {
        shader_stage_name name;
        std::string code;
    };

    struct shader_descriptor
    {
        std::vector<shader_stage> stages;
    };

    using mesh_handler = uint32_t;
    using shader_handler = uint32_t;

    class renderer
    {
    public:
        virtual mesh_handler create_mesh(const mesh_layout_descriptor&) = 0;
        virtual shader_handler create_shader(const shader_descriptor&) = 0;
        virtual void draw(mesh_handler, shader_handler) = 0;
        virtual void update(float) = 0;
    };
}

