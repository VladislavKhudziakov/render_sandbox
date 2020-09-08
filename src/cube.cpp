

#include "cube.hpp"


namespace
{
    struct vertex
    {
        float position[3];
        float normal[3];
        float color[3];
    };

    // clang-format off
    constexpr float vertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
    };
    // clang-format on

    constexpr auto vss = R"(#version 410 core
layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_n;

layout (std140) uniform instance_data
{
    vec4 color;
    mat4 mv;
    mat4 mvp;
};

out vec4 v_color;
out vec3 v_n;
out vec3 v_v;

void main()
{
    v_v = vec3(mv * vec4(attr_pos, 1.));
    gl_Position = mvp * vec4(attr_pos, 1.);
    v_n = vec3(mv * vec4(attr_n, 0.));
    v_color = vec4(v_n * 0.5 + 0.5, 1.);
}
)";

    constexpr auto fss = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

in vec4 v_color;
in vec3 v_n;
in vec3 v_v;

vec3 light_pos = vec3(10, 20, 30);

void main()
{
    float ndotl = dot(v_n, normalize(light_pos - v_v));
    frag_color = v_color;
}
)";
} // namespace


rubicks_cube::cube::cube(renderer::renderer* renderer)
    : m_renderer(renderer)
{
    renderer::parameters_list_descriptor parameters_desc{
        .parameters = {renderer::parameter_type::vec4, renderer::parameter_type::mat4, renderer::parameter_type::mat4}};

    m_params_list = m_renderer->create_parameters_list(parameters_desc);

    renderer::mesh_layout_descriptor mesh_descriptor{
        .vertex_attributes = {
            {renderer::data_type::f32, 3},
            {renderer::data_type::f32, 3}},
        .vertex_data = {reinterpret_cast<const uint8_t*>(vertices), reinterpret_cast<const uint8_t*>(vertices) + sizeof(vertices)}};

    renderer::shader_descriptor shader_descriptor{
        .stages = {
            {renderer::shader_stage_name::vertex, vss},
            {renderer::shader_stage_name::fragment, fss}},
        .samplers = {},
        .parameters = {{"instance_data", m_params_list}},
        .state = {.depth_test = renderer::depth_test_mode::less_eq}};

    m_mesh = m_renderer->create_mesh(mesh_descriptor);
    m_shader = m_renderer->create_shader(shader_descriptor);
}


rubicks_cube::cube::~cube()
{
    m_renderer->destroy_mesh(m_mesh);
    m_renderer->destroy_shader(m_shader);
    m_renderer->destroy_parameters_list(m_params_list);
}


void rubicks_cube::cube::draw()
{
    auto tr_m = math::translation(translation);
    auto scale_m = math::scale({side / default_side, side / default_side, side / default_side});

    auto transform_m = tr_m * scale_m;
    auto mvp = transform_m * parent_transform;

    m_renderer->set_parameter_data(m_params_list, 1, &transform_m[0][0]);
    m_renderer->set_parameter_data(m_params_list, 2, &mvp[0][0]);

    float color[] {1, 1, 0, 1};
    m_renderer->set_parameter_data(m_params_list, 0, color);
    m_renderer->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = m_mesh, .shader = m_shader});
}


rubicks_cube::cube::cube(rubicks_cube::cube&& src) noexcept
{
    *this = std::move(src);
}


rubicks_cube::cube& rubicks_cube::cube::operator=(rubicks_cube::cube&& src) noexcept
{
    if (this == &src) {
        return *this;
    }

    m_mesh = src.m_mesh;
    m_shader = src.m_shader;
    m_params_list = src.m_params_list;
    m_renderer = src.m_renderer;

    src.m_mesh = renderer::null;
    src.m_shader = renderer::null;
    src.m_params_list = renderer::null;

    return *this;
}
