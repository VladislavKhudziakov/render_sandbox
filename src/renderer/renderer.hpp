


#pragma once

#include <cinttypes>
#include <vector>
#include <unordered_map>
#include <string>

namespace renderer
{
    using mesh_handler = uint32_t;
    using shader_handler = uint32_t;
    using texture_handler = uint32_t;

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

    enum class texture_format
    {
        r, rg, rgba
    };

    enum class texture_type
    {
        d2, d3, cube
    };

    enum class texture_filtration
    {
        point, bilinear, trilinear
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
        std::unordered_map<std::string, texture_handler> samplers;
    };

    struct texture_size
    {
        size_t width, height, depth = 0, length = 1;
    };

    struct texture_descriptor
    {
        type data_type;
        texture_format format;
        texture_type type;
        texture_size size;
        texture_filtration filtration;
        std::vector<uint8_t> pixels_data;
        bool mips = false;
    };

    class renderer
    {
    public:
        virtual mesh_handler create_mesh(const mesh_layout_descriptor&) = 0;
        virtual shader_handler create_shader(const shader_descriptor&) = 0;
        virtual texture_handler create_texture(const texture_descriptor&) = 0;
        virtual void set_shader_sampler(shader_handler, texture_handler, const std::string&) = 0;
        virtual void draw(mesh_handler, shader_handler) = 0;
        virtual void update(float) = 0;
    };
}

