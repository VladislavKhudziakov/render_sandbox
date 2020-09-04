


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
    using parameters_list_handler = uint32_t;
    using pass_handler = uint32_t;

    enum class data_type
    {
        f32,
        f16,
        u32,
        u16,
        u8,
        d24
    };

    enum class shader_stage_name
    {
        vertex,
        fragment
    };

    struct vertex_attribute
    {
        data_type data_type;
        uint32_t elements_count;
    };

    enum class texture_format
    {
        r,
        rg,
        rgba
    };

    enum class texture_type
    {
        d2,
        d3,
        cube,
        attachment
    };

    enum class texture_filtration
    {
        point,
        bilinear,
        trilinear
    };

    enum class parameter_type
    {
        vec4,
        mat2x4,
        mat3x4,
        mat4
    };

    enum class draw_command_type
    {
        pass,
        draw
    };

    enum class blend_mode
    {
        off,
        alpha,
        add,
        multiply
    };

    enum class cull_mode
    {
        off,
        back,
        front
    };

    enum class depth_test_mode
    {
        off,
        less,
        less_eq,
        greater,
        greater_eq
    };

    enum class pass_start_behavior
    {
        clear,
        load
    };

    enum class pass_finish_behavior
    {
        store,
        discard
    };

    enum class attachment_type
    {
        color,
        depth
    };

    struct mesh_layout_descriptor
    {
        std::vector<vertex_attribute> vertex_attributes;
        std::vector<uint8_t> vertex_data;
        std::vector<uint8_t> index_data;
        data_type indices_data_type = data_type::u16;
    };

    struct shader_stage
    {
        shader_stage_name name;
        std::string code;
    };

    struct shader_state
    {
        bool color_write = true;
        bool depth_write = true;
        depth_test_mode depth_test = depth_test_mode::off;
        blend_mode blend = blend_mode::off;
        cull_mode cull = cull_mode::off;
    };

    struct shader_descriptor
    {
        std::vector<shader_stage> stages;
        std::unordered_map<std::string, texture_handler> samplers;
        std::unordered_map<std::string, parameters_list_handler> parameters;
        shader_state state;
    };

    struct texture_size
    {
        size_t width, height, depth = 0, length = 1;
    };

    struct texture_descriptor
    {
        data_type pixels_data_type;
        texture_format format;
        texture_type type;
        texture_size size;
        texture_filtration filtration = texture_filtration::point;
        std::vector<uint8_t> pixels;
        bool mips = false;
    };

    struct parameters_list_descriptor
    {
        std::vector<parameter_type> parameters;
    };

    struct draw_command
    {
        draw_command_type type;
        mesh_handler mesh;
        shader_handler shader;
        pass_handler pass;
    };


    struct attachment_descriptor
    {
        attachment_type type;
        texture_handler render_texture;
    };


    struct pass_state
    {
        pass_start_behavior start = pass_start_behavior::clear;
        pass_finish_behavior finish = pass_finish_behavior::store;
        float clear_color[4]{0, 0, 0, 1};
        float clear_depth = -1;
    };

    struct pass_descriptor
    {
        size_t width, height;
        std::vector<attachment_descriptor> attachments;
        pass_state state;
    };

    class renderer
    {
    public:
        virtual ~renderer() = default;

        virtual mesh_handler create_mesh(const mesh_layout_descriptor&) = 0;
        virtual void destroy_mesh(mesh_handler) = 0;

        virtual shader_handler create_shader(const shader_descriptor&) = 0;
        virtual void destroy_shader(shader_handler) = 0;
        virtual void set_shader_sampler(shader_handler, texture_handler, const std::string& sampler_name) = 0;

        virtual texture_handler create_texture(const texture_descriptor&) = 0;
        virtual void destroy_texture(texture_handler) = 0;

        virtual parameters_list_handler create_parameters_list(const parameters_list_descriptor&) = 0;
        virtual void destroy_parameters_list(parameters_list_handler) = 0;

        virtual void set_parameter_data(parameters_list_handler, uint32_t parameter_index, void* data) = 0;

        virtual pass_handler create_pass(const pass_descriptor&) = 0;
        virtual void destroy_pass(pass_handler) = 0;
        virtual void resize_pass(pass_handler, size_t w, size_t h) = 0;

        virtual void encode_draw_command(draw_command) = 0;
        virtual void update(float) = 0;
    };
} // namespace renderer
