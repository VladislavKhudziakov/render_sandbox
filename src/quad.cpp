

#include "quad.hpp"

#include <misc/images_loader.hpp>

namespace
{
    // clang-format off
    constexpr float vertices[] = {
        0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
    };
    // clang-format on


    constexpr auto vss = R"(#version 410 core
layout (location = 0) in vec3 attr_v;
layout (location = 1) in vec2 attr_uv;

out vec2 var_uv;

layout (std140) uniform instance_data
{
    mat4 mvp;
};

void main()
{
    var_uv = attr_uv;
    var_uv.y = 1. - var_uv.y;
    gl_Position = mvp * vec4(attr_v, 1.);
}
)";


    constexpr auto fss = R"(#version 410 core
    layout (location = 0) out vec4 frag_color;

in vec2 var_uv;

uniform sampler2D s_tex;

void main()
{
    frag_color = texture(s_tex, var_uv);
}
)";
} // namespace



rubiks_cube::indication_quad::indication_quad(renderer::renderer* r)
    : m_renderer(r)
{
    misc::images_loader loader;

    m_draw_texture = m_renderer->create_texture(loader.load_2d_texture("./resources/mlg.png"));

    renderer::parameters_list_descriptor params_list_descriptor {
        .parameters = {renderer::parameter_type::mat4}
    };
    m_params_list = m_renderer->create_parameters_list(params_list_descriptor);

    renderer::shader_descriptor draw_shader_descriptor {
        .stages = {
            renderer::shader_stage {
                .name = renderer::shader_stage_name::vertex,
                .code = vss
            },
            renderer::shader_stage {
                .name = renderer::shader_stage_name::fragment,
                .code = fss
            }
        },
        .samplers = {
            {"s_tex", m_draw_texture}
        },
        .parameters = {
            {"instance_data", m_params_list}
        },
    };

    m_shader = m_renderer->create_shader(draw_shader_descriptor);

    renderer::mesh_layout_descriptor mesh_descriptor{
        .vertex_attributes = {
            {renderer::data_type::f32, 3}, {renderer::data_type::f32, 2}},
        .vertex_data = {reinterpret_cast<const uint8_t*>(vertices), reinterpret_cast<const uint8_t*>(vertices) + sizeof(vertices)}};

    m_mesh = m_renderer->create_mesh(mesh_descriptor);
}


rubiks_cube::indication_quad::~indication_quad()
{
    m_renderer->destroy_texture(m_draw_texture);
    m_renderer->destroy_shader(m_shader);
    m_renderer->destroy_parameters_list(m_params_list);
    m_renderer->destroy_mesh(m_mesh);
}


void rubiks_cube::indication_quad::update()
{
    auto translation_m = math::translation(position);
    auto scale_m = math::scale(scale);
    auto transform = math::transpose(translation_m * scale_m);
    m_renderer->set_parameter_data(m_params_list, 0, &transform[0][0]);
}


void rubiks_cube::indication_quad::draw()
{
    m_renderer->encode_draw_command({.mesh = m_mesh, .shader = m_shader, .instances_count = 1});
}

