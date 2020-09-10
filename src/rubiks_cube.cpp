

#include "rubiks_cube.hpp"

#include <sstream>



namespace
{
    std::string generate_cubes_vert_shader(size_t cubes_count)
    {
        std::ostringstream oss;
        oss << R"(#version 410 core
layout (location = 0) in vec3 attr_pos;

layout (std140) uniform instance_data
{
    mat4 mvp)" << "[" << cubes_count << "];\n"
<< R"(
};

out vec4 v_color;

void main()
{
    gl_Position = mvp[gl_InstanceID] * vec4(attr_pos, 1.);
    v_color = vec4(gl_VertexID / 36.0f, gl_VertexID / 36.0f, 0., 1.);
})";
        return oss.str();
    }

   constexpr auto fss = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

in vec4 v_color;

vec3 light_pos = vec3(10, 20, 30);

void main()
{
    frag_color = v_color;
}
)";

    // clang-format off
    constexpr float vertices[] = {
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
    };
// clang-format on
}


rubiks_cube::rubiks_cube::rubiks_cube(renderer::renderer* renderer, size_t size)
    : m_size(size)
    , rotation_manager(size)
    , m_renderer(renderer)
{
    float rubiks_cube_size = size * 2;
    float offset = 2 * 0.5f;
    float x_pos = -rubiks_cube_size / 2 + offset;
    float y_pos = -rubiks_cube_size / 2 + offset;
    float z_pos = rubiks_cube_size / 2 - offset;
    float stride = 2;

    m_cubes.reserve(size * size * size);

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            for (int z = 0; z < size; ++z) {
                auto& new_cube = m_cubes.emplace_back(
                    math::ivec3{int32_t(x_pos), int32_t(y_pos), int32_t(z_pos)});
                z_pos -= stride;
            }
            z_pos = rubiks_cube_size / 2 - offset;
            y_pos += stride;
        }
        y_pos = -rubiks_cube_size / 2 + offset;
        x_pos += stride;
    }

    renderer::parameters_list_descriptor params_list_descriptor {
        .parameters = {m_cubes.size(),renderer::parameter_type::mat4}
    };

    m_params_list = m_renderer->create_parameters_list(params_list_descriptor);

    renderer::shader_descriptor shader_descriptor {
        .stages = {
            {
                .name = renderer::shader_stage_name::vertex,
                .code = generate_cubes_vert_shader(m_cubes.size()),
            },
            {
                .name = renderer::shader_stage_name::fragment,
                .code = fss,
            }
        },
        .samplers = {},
        .parameters = {{"instance_data", m_params_list}},
        .state = {
            .depth_test = renderer::depth_test_mode::less_eq
        }
    };

    m_draw_shader = m_renderer->create_shader(shader_descriptor);

    renderer::mesh_layout_descriptor mesh_descriptor{
        .vertex_attributes = {
            {renderer::data_type::f32, 3}},
        .vertex_data = {reinterpret_cast<const uint8_t*>(vertices), reinterpret_cast<const uint8_t*>(vertices) + sizeof(vertices)}};

    m_cube_mesh = m_renderer->create_mesh(mesh_descriptor);
}


void rubiks_cube::rubiks_cube::draw()
{
    m_renderer->encode_draw_command({
        .type = ::renderer::draw_command_type::draw,
        .mesh = m_cube_mesh,
        .shader = m_draw_shader,
        .instances_count = uint32_t(m_cubes.size())});
}


void rubiks_cube::rubiks_cube::update()
{
    rotation_manager.update();

    auto rot_m = math::rotation_x(rotation.x) * math::rotation_y(rotation.y) * math::rotation_z(rotation.z);
    auto t_m = math::translation(translation);
    auto s_m = math::scale(scale);

    auto self_transform = rot_m * t_m * s_m;

    for (int i = 0; i < m_cubes.size(); ++i) {
        auto& cube = m_cubes[i];
        auto cube_transform = cube.get_transformation() * self_transform * parent_transform;
        m_renderer->set_parameter_data(m_params_list, i, &cube_transform[0][0]);
//        auto pos = cube.get_position();
//        pos.x = pos.x / 2 + (m_size) / 2;
//        pos.y = pos.y / 2 + (m_size) / 2;
//        pos.z = pos.z / 2 + (m_size) / 2;
//        pos = pos;
    }
}
