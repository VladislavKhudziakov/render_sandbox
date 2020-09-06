
#include <window/glfw_window.hpp>
#include <renderer/renderer.hpp>
#include <misc/images_loader.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>
#include <iostream>

constexpr auto vss = R"(#version 410 core
layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec2 attr_uv;

layout (std140) uniform instance_data
{
    vec4 color;
    mat4 test_matrix;
};

out vec2 v_uv;
out vec4 v_color;

void main()
{
    gl_Position = vec4(attr_pos.xy, 0., 1.);
    v_uv = attr_uv;
    v_color = color;
}
)";

constexpr auto fss = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

in vec2 v_uv;
in vec4 v_color;

uniform sampler2D s_tex;

void main()
{
    frag_color = texture(s_tex, v_uv) * v_color;
}
)";

// clang-format off
constexpr float arr[]{
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.5f, 1.0f,
    0.0f, 0.5f, 0.0f, 1.0f, 0.0f
};
// clang-format on

int main()
{
    misc::images_loader loader;

    renderer::mesh_handler mesh;
    renderer::shader_handler shader;

    renderer::glfw_window window("my window", {800, 600});

    auto* r = window.get_renderer();

    renderer::mesh_layout_descriptor md{
        .vertex_attributes = {
            {renderer::data_type::f32, 3},
            {renderer::data_type::f32, 2}},
        .vertex_data = {reinterpret_cast<const uint8_t*>(arr), reinterpret_cast<const uint8_t*>(arr) + sizeof(arr)}};

    auto image = r->create_texture(loader.load_2d_texture("/Users/vladislavkhudiakov/Downloads/test_image.png"));

    renderer::parameters_list_descriptor parameters_list{
        .parameters = {renderer::parameter_type::vec4, renderer::parameter_type::mat4}};

    auto params_list = r->create_parameters_list(parameters_list);

    renderer::shader_descriptor sd{
        .stages = {
            {renderer::shader_stage_name::vertex, vss},
            {renderer::shader_stage_name::fragment, fss}},
        .samplers = {{"s_tex", image}},
        .parameters = {{"instance_data", params_list}},
    };

    renderer::texture_descriptor attachment_tex_descriptor{
        .pixels_data_type = renderer::data_type::u8,
        .format = renderer::texture_format::rgba,
        .type = renderer::texture_type::attachment,
        .size = {
            1, 1, 0, 1}};

    auto attachment_tex = r->create_texture(attachment_tex_descriptor);

    renderer::pass_descriptor pass_descriptor{
        .width = 1600,
        .height = 1200,
        .attachments = {{.type = renderer::attachment_type::color, .render_texture = attachment_tex}}};

    auto pass = r->create_pass(pass_descriptor);

    mesh = r->create_mesh(md);
    shader = r->create_shader(sd);

    float color[] = {1, 1, 0, 1};

    // clang-format off
    float matrix[] = {
        1, 0, 0, 1,
        0, 1, 0, 1,
        0, 0, 1, 1,
        1, 0, 1, 1,
    };
    // clang-format on

    r->set_parameter_data(params_list, 0, color);
    r->set_parameter_data(params_list, 1, matrix);

    math::vec4 v1 {1, 2, 3, 4};
    math::vec4 v2 {1, 2, 3, 4};

    math::mat4 m {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    math::mat4 m2 {
        1.f, 4.f, 0.f, 0.f,
        0.f, 2.f, 0.f, 0.f,
        0.f, 0.f, 3.f, 0.f,
        0.f, 0.f, 0.f, 4.f
    };


    auto m3 = m * m2;
    auto m4 = m2 * m;

    auto res = m * v1;
    auto res2 = v1 * m;

    auto m5 = m + 2;
    auto m6 = m + m5;

    while (!window.closed()) {
        r->encode_draw_command({.type = renderer::draw_command_type::pass, .pass = pass});
        r->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = mesh, .shader = shader});
        window.update();
    }

    return 0;
}
