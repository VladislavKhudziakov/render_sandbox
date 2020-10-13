

#include "rubiks_cube.hpp"

#include <misc/images_loader.hpp>
#include <math/matrix_operations.hpp>

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
            << R"(
};


uniform sampler2D s_faces_colors;

out vec4 v_color;

void main()
{
    gl_Position = mvp[gl_InstanceID] * vec4(attr_pos, 1.);
    v_color = texelFetch(s_faces_colors, ivec2(gl_VertexID / 6, gl_InstanceID), 0);
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
    if (size % 2 == 0) {
        throw std::runtime_error("don't support even size.");
    }
    int rubiks_cube_size = int(size);
    int x_pos = -rubiks_cube_size / 2;
    int y_pos = -rubiks_cube_size / 2;
    int z_pos = -rubiks_cube_size / 2;
    int stride = 1;

    m_cubes.reserve(size * size * size);

    renderer::parameters_list_descriptor params_list_descriptor{
        .parameters = {size * size * size, renderer::parameter_type::mat4}};

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            for (int z = 0; z < size; ++z) {
                auto& new_cube = m_cubes.emplace_back(
                    math::ivec3{int32_t(x_pos), int32_t(y_pos), int32_t(z_pos)});
                new_cube.color = {float(x) / float(m_size), float(y) / float(m_size), float(z) / float(m_size), 1.f};
                get_faces_color_by_position(new_cube, new_cube.get_position());
                z_pos += stride;
            }
            z_pos = -rubiks_cube_size / 2;
            y_pos += stride;
        }
        y_pos = -rubiks_cube_size / 2;
        x_pos += stride;
    }

    create_cubes_colors_texture();

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
        .samplers = {{"s_faces_colors", m_cubes_faces_texture}},
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


void rubiks_cube::rubiks_cube::update(double delta_time)
{
    rotation_manager.update(delta_time);

    auto rot_m = math::rotation_x(m_rotation.x) * math::rotation_y(m_rotation.y);
    m_transform = rot_m;

    math::mat4 m = parent_transform * m_transform;

    for (int i = 0; i < m_cubes.size(); ++i) {
        auto& cube = m_cubes[i];
        rotation_manager.rotate_cube(cube);
        auto cube_transform = math::transpose(m * cube.get_transformation());
        m_renderer->set_parameter_data(m_params_list, i, &cube_transform[0][0]);
    }

    // i know that this is BAD solution. But deadline is coming...
    upload_cube_faces_texture_data();
    m_renderer->load_texture_data(m_cubes_faces_texture, {6, m_cubes.size(), 1}, m_faces_texture_data.data());
}


bool rubiks_cube::rubiks_cube::hit(math::ray ray, face& face, math::vec3& out_hit_point)
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

    face = rubiks_cube::face(intersection_face);
    out_hit_point = intersection_hit_point;

    return intersection_success;
}


math::ivec2 rubiks_cube::rubiks_cube::get_row_col_by_hit_pos(face& face, math::vec3 hit_point)
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
                    res.x = (m_size - 1) - x; // in reverse order
                }
                break;
            case pos_y:
                [[fallthrough]];
            case neg_y:
                if (hit_point.x >= x_min && hit_point.x <= x_min + 1) {
                    res.y = x; // yes y = x
                }
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
                    res.x = (m_size - 1) - y;
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


math::ivec2 rubiks_cube::rubiks_cube::get_face_rotation_axis(rubiks_cube::rubiks_cube::face face)
{
    switch (face) {
        case pos_x:
            [[fallthrough]];
        case neg_x:
            return {rotation_manager::axis::y, rotation_manager::axis::z};
        case pos_y:
            [[fallthrough]];
        case neg_y:
            return {rotation_manager::axis::x, rotation_manager::axis::z};
        case pos_z:
            [[fallthrough]];
        case neg_z:
            return {rotation_manager::axis::y, rotation_manager::axis::x};
        default:
            throw std::runtime_error("invalid axis");
    }
}


void rubiks_cube::rubiks_cube::acquire()
{
    m_is_acquired = true;
}


void rubiks_cube::rubiks_cube::release()
{
    m_is_acquired = false;
}


bool rubiks_cube::rubiks_cube::is_acquired() const
{
    return m_is_acquired;
}


void rubiks_cube::rubiks_cube::rotate(math::vec3 angles)
{
    if (m_is_acquired) {
        m_rotation = m_rotation + angles;
    }
}


void rubiks_cube::rubiks_cube::create_cubes_colors_texture()
{
    m_faces_texture_data.resize(6 * m_cubes.size());
    upload_cube_faces_texture_data();

    auto begin = reinterpret_cast<uint8_t*>(m_faces_texture_data.data());
    auto end = begin + m_faces_texture_data.size() * sizeof(math::vec4);

    renderer::texture_descriptor descriptor{
        .pixels_data_type = renderer::data_type::u8,
        .format = renderer::texture_format::rgba,
        .type = renderer::texture_type::d2,
        .size = {
            .width = 6,
            .height = m_cubes.size(),
            .length = 1},
        .pixels = {begin, end}};

    m_cubes_faces_texture = m_renderer->create_texture(descriptor);
}


void rubiks_cube::rubiks_cube::get_faces_color_by_position(cube& cube, math::ivec3 pos)
{
    const auto min_pos = -int(m_size) / 2;
    const auto max_pos = int(m_size) / 2;

    if (pos.x == min_pos) {
        cube.faces[0].color = {255, 0, 0, 255};
        cube.faces[1].color = {0, 0, 0, 255};
    } else if (pos.x == max_pos) {
        cube.faces[0].color = {0, 0, 0, 255};
        cube.faces[1].color = {0, 255, 0, 255};
    } else {
        cube.faces[0].color = {0, 0, 0, 255};
        cube.faces[1].color = {0, 0, 0, 255};
    }

    if (pos.y == min_pos) {
        cube.faces[2].color = {0, 0, 255, 255};
        cube.faces[3].color = {0, 0, 0, 255};
    } else if (pos.y == max_pos) {
        cube.faces[2].color = {0, 0, 0, 255};
        cube.faces[3].color = {255, 255, 0, 255};
    } else {
        cube.faces[2].color = {0, 0, 0, 255};
        cube.faces[3].color = {0, 0, 0, 255};
    }

    if (pos.z == min_pos) {
        cube.faces[4].color = {255, 175, 0, 255};
        cube.faces[5].color = {0, 0, 0, 255};
    } else if (pos.z == max_pos) {
        cube.faces[4].color = {0, 0, 0, 255};
        cube.faces[5].color = {255, 255, 255, 255};
    } else {
        cube.faces[4].color = {0, 0, 0, 255};
        cube.faces[5].color = {0, 0, 0, 255};
    }
}


void rubiks_cube::rubiks_cube::upload_cube_faces_texture_data()
{
    for (int i = 0; i < m_cubes.size(); ++i) {
        auto& cube = m_cubes[i];

        for (const auto& face : cube.faces) {
            auto index = abs(face.normal.x) > 0 ? face.normal.x * 0.5 + 0.5 : 0;
            index += abs(face.normal.y) > 0 ? 2 + face.normal.y * 0.5 + 0.5 : 0;
            index += abs(face.normal.z) > 0 ? 4 + face.normal.z * 0.5 + 0.5 : 0;
            m_faces_texture_data[i * 6 + index] = face.color;
        }
    }
}


rubiks_cube::rubiks_cube::~rubiks_cube()
{
    m_renderer->destroy_parameters_list(m_params_list);
    m_renderer->destroy_shader(m_draw_shader);
    m_renderer->destroy_texture(m_cubes_faces_texture);
}


size_t rubiks_cube::rubiks_cube::get_size()
{
    return m_size;
}


bool rubiks_cube::rubiks_cube::is_assembled() const
{
    const auto min_pos = -int(m_size) / 2;
    const auto max_pos = int(m_size) / 2;

    size_t neg_x_iters = 0;
    size_t pos_x_iters = 0;

    math::ubvec4 last_neg_x_color;
    math::ubvec4 last_pos_x_color;

    size_t neg_y_iters = 0;
    size_t pos_y_iters = 0;

    math::ubvec4 last_neg_y_color;
    math::ubvec4 last_pos_y_color;

    size_t neg_z_iters = 0;
    size_t pos_z_iters = 0;

    math::ubvec4 last_neg_z_color;
    math::ubvec4 last_pos_z_color;

    auto check_cube_face_color = [](math::vec4 normal, const cube& cube, math::ubvec4& check_color, size_t& iterations_counter) {
        for (const auto& face : cube.faces) {
            if (face.normal == normal) {
                if (iterations_counter == 0) {
                    check_color = face.color;
                }
                iterations_counter++;
                return face.color == check_color;
            }
        }
        return false;
    };

    for (auto& cube : m_cubes) {
        auto pos = cube.get_position();

        if (pos.x == min_pos) {
            if (!check_cube_face_color({-1, 0, 0, 0}, cube, last_neg_x_color, neg_x_iters)) {
                return false;
            }
        }

        if (pos.x == max_pos) {
            if (!check_cube_face_color({1, 0, 0, 0}, cube, last_pos_x_color, pos_x_iters)) {
                return false;
            }
        }

        if (pos.y == min_pos) {
            if (!check_cube_face_color({0, -1, 0, 0}, cube, last_neg_y_color, neg_y_iters)) {
                return false;
            }
        }

        if (pos.y == max_pos) {
            if (!check_cube_face_color({0, 1, 0, 0}, cube, last_pos_y_color, pos_y_iters)) {
                return false;
            }
        }

        if (pos.z == min_pos) {
            if (!check_cube_face_color({0, 0, -1, 0}, cube, last_neg_z_color, neg_z_iters)) {
                return false;
            }
        }

        if (pos.z == min_pos) {
            if (!check_cube_face_color({0, 0, 1, 0}, cube, last_pos_z_color, pos_z_iters)) {
                return false;
            }
        }
    }

    return true;
}
