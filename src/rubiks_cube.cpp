

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
    mat4 mvp)"
            << "[" << cubes_count << "];\n"
            << "\tvec4 cube_color"
            << "[" << cubes_count << "];\n"
            << R"(
};

out vec4 v_color;

void main()
{
    gl_Position = mvp[gl_InstanceID] * vec4(attr_pos, 1.);
    v_color = cube_color[gl_InstanceID];
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
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
    };
    // clang-format on
} // namespace


rubiks_cube::rubiks_cube::rubiks_cube(renderer::renderer* renderer, size_t size)
    : m_size(size)
    , rotation_manager(size)
    , m_renderer(renderer)
{
    int rubiks_cube_size = int(size);
    int x_pos = -rubiks_cube_size / 2;
    int y_pos = -rubiks_cube_size / 2;
    int z_pos = rubiks_cube_size / 2;
    int stride = 1;

    m_cubes.reserve(size * size * size);

    renderer::parameters_list_descriptor params_list_descriptor{
        .parameters = {size * size * size, renderer::parameter_type::mat4}};
    params_list_descriptor.parameters.reserve(params_list_descriptor.parameters.size() + size * size * size);

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            for (int z = 0; z < size; ++z) {
                auto& new_cube = m_cubes.emplace_back(
                    math::ivec3{int32_t(x_pos), int32_t(y_pos), int32_t(z_pos)});
                new_cube.color = {float(x) / float(m_size), float(y) / float(m_size), float(z) / float(m_size), 1.f};
                z_pos -= stride;
                params_list_descriptor.parameters.emplace_back(::renderer::parameter_type::vec4);
            }
            z_pos = rubiks_cube_size / 2;
            y_pos += stride;
        }
        y_pos = -rubiks_cube_size / 2;
        x_pos += stride;
    }

    m_params_list = m_renderer->create_parameters_list(params_list_descriptor);

    renderer::shader_descriptor shader_descriptor{
        .stages = {
            {
                .name = renderer::shader_stage_name::vertex,
                .code = generate_cubes_vert_shader(m_cubes.size()),
            },
            {
                .name = renderer::shader_stage_name::fragment,
                .code = fss,
            }},
        .samplers = {},
        .parameters = {{"instance_data", m_params_list}},
        .state = {.depth_test = renderer::depth_test_mode::less_eq}};

    m_draw_shader = m_renderer->create_shader(shader_descriptor);

    renderer::mesh_layout_descriptor mesh_descriptor{
        .vertex_attributes = {
            {renderer::data_type::f32, 3}},
        .vertex_data = {reinterpret_cast<const uint8_t*>(vertices), reinterpret_cast<const uint8_t*>(vertices) + sizeof(vertices)}};

    m_cube_mesh = m_renderer->create_mesh(mesh_descriptor);
}


void rubiks_cube::rubiks_cube::draw()
{
    m_renderer->encode_draw_command({.type = ::renderer::draw_command_type::draw, .mesh = m_cube_mesh, .shader = m_draw_shader, .instances_count = uint32_t(m_cubes.size())});
}


void rubiks_cube::rubiks_cube::update()
{
    rotation_manager.update();

    auto rot_m = math::rotation_x(rotation.x) * math::rotation_y(rotation.y);
    m_transform = rot_m;

    math::mat4 m = parent_transform * m_transform;

    for (int i = 0; i < m_cubes.size(); ++i) {
        auto& cube = m_cubes[i];
        rotation_manager.rotate_cube(cube);
        auto cube_transform = math::transpose(m * cube.get_transformation());
        m_renderer->set_parameter_data(m_params_list, i, &cube_transform[0][0]);
        auto index = m_size * m_size * m_size + i;
        m_renderer->set_parameter_data(m_params_list, index, &cube.color.x);
    }
}


bool rubiks_cube::rubiks_cube::hit(math::ray ray, faces& face, math::vec3& out_hit_point)
{
    static math::vec3 vertices[] = {
        // pos x
        {0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, 0.5f, 0.5f},

        // neg x
        {-0.5f, 0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f},

        // pos y
        {-0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, -0.5f},

        // neg y
        {-0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f},

        // pos z
        {-0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f},

        // neg z
        {-0.5f, 0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
    };

    std::vector<int> hit_surfaces;

    auto world_to_model = math::inverse(m_transform);

    math::vec4 ray_model_from{ray.from.x, ray.from.y, ray.from.z, 1};
    ray_model_from = world_to_model * ray_model_from;

    math::vec4 ray_model_to{ray.to.x, ray.to.y, ray.to.z, 1};
    ray_model_to = world_to_model * ray_model_to;

    ray.from = {ray_model_from.x, ray_model_from.y, ray_model_from.z};
    ray.to = {ray_model_to.x, ray_model_to.y, ray_model_to.z};

    bool intersection_success = false;
    math::vec3 intersection_hit_point;
    uint32_t intersection_face = 0;

    for (int i = 0; i < std::size(vertices); i += 4) {
        auto e1 = vertices[i + 1] - vertices[i];
        auto e2 = vertices[i + 3] - vertices[i];
        auto n = math::normalize(math::cross(e1, e2));
        math::vec3 hit_point;
        auto hit_success = math::hit_with_surface(ray, vertices[i] * m_size, n, hit_point);

        if (hit_success) {
            math::vec3 scaled_vertices[]{
                vertices[i] * m_size,
                vertices[i + 1] * m_size,
                vertices[i + 2] * m_size,
                vertices[i + 3] * m_size,
            };

            if (math::is_point_inside_polygon(hit_point, scaled_vertices, 4)) {
                if (!intersection_success) {
                    intersection_hit_point = hit_point;
                    intersection_success = true;
                    intersection_face = i / 4;
                } else {
                    auto dist = math::length(hit_point - ray.from);
                    auto last_dist = math::length(intersection_hit_point - ray.from);
                    if (dist < last_dist) {
                        intersection_face = i / 4;
                        intersection_hit_point = hit_point;
                    }
                }
            }
        }
    }

    face = static_cast<faces>(intersection_face);
    out_hit_point = intersection_hit_point;

    return intersection_success;
}


math::ivec2 rubiks_cube::rubiks_cube::get_row_col_by_hit_pos(faces& face, math::vec3 hit_point)
{
    float x_min = -float(m_size) / 2;
    float y_min = -float(m_size) / 2;

    math::ivec2 res;

    for (int x = 0; x < m_size; ++x) {
        switch (face) {
            case pos_x:
                [[fallthrough]];
            case neg_x:
                if (hit_point.z >= x_min && hit_point.z <= x_min + 1) {
                    res.x = x;
                }
                break;
            case pos_y:
                [[fallthrough]];
            case neg_y:
                [[fallthrough]];
            case pos_z:
                [[fallthrough]];
            case neg_z:
                if (hit_point.x >= x_min && hit_point.x <= x_min + 1) {
                    res.x = x;
                }
                break;
            default:
                throw std::runtime_error("invalid face");
        }

        x_min += 1;
    }

    for (int y = 0; y < m_size; ++y) {
        switch (face) {
            case pos_y:
                [[fallthrough]];
            case neg_y:
                if (hit_point.z >= y_min && hit_point.z <= y_min + 1) {
                    res.y = y;
                }
                break;
            case pos_x:
                [[fallthrough]];
            case neg_x:
                [[fallthrough]];
            case pos_z:
                [[fallthrough]];
            case neg_z:
                if (hit_point.y >= y_min && hit_point.y <= y_min + 1) {
                    res.y = y;
                }
                break;
            default:
                throw std::runtime_error("invalid face");
        }

        y_min += 1;
    }

    return res;
}
