

#include "cube.hpp"


namespace
{
    struct vertex
    {
        float position[3];
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


    constexpr vertex colored_vertices[] {
        // pos x
        {
            {1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            { 1.0f,  1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            {1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            { 1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            { 1.0f, -1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            {1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },

        // neg x
        {
            {-1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            { -1.0f,  1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            {-1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            { -1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            { -1.0f, -1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            {-1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },

        // pos y
        {
            {-1.0f,  1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            { 1.0f,  1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            {1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            { 1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            {  -1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            { -1.0f,  1.0f, -1.0f},
            {0, 0, 0}
        },

        // neg y
        {
            {-1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            { 1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            {1.0f, -1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            {  1.0f, -1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            {   -1.0f, -1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            { -1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        },

        // pos z
        {
            {-1.0f, -1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            {  1.0f, -1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            {1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            {  1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            {   -1.0f,  1.0f,  1.0f},
            {0, 0, 0}
        },
        {
            { -1.0f, -1.0f,  1.0f},
            {0, 0, 0}
        },

        // neg z
        {
            {-1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            {  1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            {1.0f,  1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            {  1.0f,  1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            {   -1.0f,  1.0f, -1.0f},
            {0, 0, 0}
        },
        {
            { -1.0f, -1.0f, -1.0f},
            {0, 0, 0}
        }
    };

    // clang-format on

    constexpr auto vss = R"(#version 410 core
layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_color;

layout (std140) uniform instance_data
{
    mat4 mv;
    mat4 mvp;
};

out vec4 v_color;

void main()
{
    gl_Position = mvp * vec4(attr_pos, 1.);
    v_color = vec4(attr_color, 1.);
}
)";

    constexpr auto fss = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

in vec4 v_color;

vec3 light_pos = vec3(10, 20, 30);

void main()
{
    frag_color = v_color;
}
)";
} // namespace


rubiks_cube::cube::cube(renderer::renderer* renderer)
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


rubiks_cube::cube::cube(renderer::renderer* renderer, const rubiks_cube::color_data& color)
    : m_renderer(renderer)
{
    std::vector<vertex> cube_vertices;
    cube_vertices.resize(36);
    std::memcpy(cube_vertices.data(), colored_vertices, sizeof(colored_vertices));

    for (int i = 0; i < 6; ++i) {
        std::memcpy(cube_vertices[i].color, color.pos_x, sizeof(color.pos_x));
    }

    for (int i = 6; i < 12; ++i) {
        std::memcpy(cube_vertices[i].color, color.neg_x, sizeof(color.neg_x));
    }

    for (int i = 12; i < 18; ++i) {
        std::memcpy(cube_vertices[i].color, color.pos_y, sizeof(color.pos_y));
    }

    for (int i = 18; i < 24; ++i) {
        std::memcpy(cube_vertices[i].color, color.neg_y, sizeof(color.neg_y));
    }

    for (int i = 24; i < 30; ++i) {
        std::memcpy(cube_vertices[i].color, color.pos_z, sizeof(color.pos_z));
    }

    for (int i = 30; i < 36; ++i) {
        std::memcpy(cube_vertices[i].color, color.neg_z, sizeof(color.neg_z));
    }


    renderer::parameters_list_descriptor parameters_desc{
        .parameters = {renderer::parameter_type::mat4, renderer::parameter_type::mat4}};

    m_params_list = m_renderer->create_parameters_list(parameters_desc);

    renderer::mesh_layout_descriptor mesh_descriptor{
        .vertex_attributes = {
            {renderer::data_type::f32, 3},
            {renderer::data_type::f32, 3}},
        .vertex_data = {reinterpret_cast<const uint8_t*>(cube_vertices.data()), reinterpret_cast<const uint8_t*>(cube_vertices.data()) + cube_vertices.size() * sizeof(vertex)}};

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


rubiks_cube::cube::~cube()
{
    m_renderer->destroy_mesh(m_mesh);
    m_renderer->destroy_shader(m_shader);
    m_renderer->destroy_parameters_list(m_params_list);
}


void rubiks_cube::cube::draw()
{
    auto tr_m = math::translation(translation);
    auto scale_m = math::scale({side / default_side, side / default_side, side / default_side});

    auto transform_m = tr_m * scale_m;
    auto mvp = transform_m * parent_transform;

    m_renderer->set_parameter_data(m_params_list, 0, &transform_m[0][0]);
    m_renderer->set_parameter_data(m_params_list, 1, &mvp[0][0]);
    m_renderer->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = m_mesh, .shader = m_shader});
}


rubiks_cube::cube::cube(rubiks_cube::cube&& src) noexcept
{
    *this = std::move(src);
}


rubiks_cube::cube& rubiks_cube::cube::operator=(rubiks_cube::cube&& src) noexcept
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
