
#include <math/quaternion.hpp>
#include <math/misc/misc.hpp>
#include <math/bound_boxes/bound.hpp>
#include <misc/images_loader.hpp>

#include <window/glfw_window.hpp>
#include <renderer/renderer.hpp>
#include <renderer/camera.hpp>
#include <math/matrix_operations.hpp>
#include <math/raytracing/ray.hpp>
#include <shapes/sphere.hpp>
#include <shapes/cylinder.hpp>

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

constexpr auto vs = R"(#version 410 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec2 attr_uv;
layout (location = 2) in vec3 attr_normal;
layout (location = 3) in vec3 attr_tangent;

layout (std140) uniform instance_data
{
    mat4 mvp;
    mat4 model_matrix;
    mat4 normal_matrix;
    mat4 vp_matrix;
};

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec3 v_pos;
out mat4 v_vp_matrix;

void main()
{
    gl_Position = mvp * vec4(attr_pos, 1.);
    v_normal = mat3(normal_matrix) * attr_normal;
    v_tangent = mat3(normal_matrix) * attr_tangent;
    v_bitangent = mat3(normal_matrix) * cross(attr_normal, attr_tangent);
    v_pos = vec3(model_matrix * vec4(attr_pos, 1.));
    v_uv = attr_uv;
    v_uv.y = 1. - v_uv.y;
    v_vp_matrix = vp_matrix;
})";


constexpr auto normal_gs = R"(#version 410 core
layout(triangles) in;
layout(line_strip, max_vertices = 36) out;

layout (std140) uniform instance_data
{
    mat4 mvp;
    mat4 model_matrix;
    mat4 normal_matrix;
    mat4 vp_matrix;
};

in vec2 v_uv[3];
in vec3 v_normal[3];
in vec3 v_tangent[3];
in vec3 v_bitangent[3];
in vec3 v_pos[3];

out vec3 g_color;

void emit_line(vec3 color, vec3 start_pos, vec3 end_pos)
{
    g_color = color;
    gl_Position = vp_matrix * vec4(start_pos, 1.);
    EmitVertex();
    g_color = color;
    gl_Position = vp_matrix * vec4(end_pos, 1.);
    g_color = color;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    for (int i = 0; i < 3; ++i) {
        emit_line(vec3(1, 0, 0), v_pos[i], v_pos[i] + v_normal[i] * 0.1);
        emit_line(vec3(0, 1, 0), v_pos[i], v_pos[i] + v_tangent[i] * 0.1);
        emit_line(vec3(0, 0, 1), v_pos[i], v_pos[i] + v_bitangent[i] * 0.1);
    }
}
)";

constexpr auto normal_fs = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

in vec3 g_color;

void main()
{
    frag_color = vec4(g_color, 1.);
}
)";

constexpr auto fs = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

in vec2 v_uv;
in vec3 v_normal;
in vec3 v_tangent;

uniform sampler2D s_uv_map;

vec3 light_dir = normalize(vec3(3, 3, 3) - vec3(0, 0, 0));

void main()
{
    float c = max(dot(v_normal, light_dir), 0.0);
    frag_color = texture(s_uv_map, v_uv) * c;
}
)";

bool mouse_clicked = false;

int main()
{
    math::quaternion q = math::quaternion_rotation({0, 0, 1}, 0);
    math::bound_boxes::bound3 bound {{-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5}};
    math::vec2 mouse_position{};

    renderer::glfw_window window("my window", {800, 600});

    renderer::camera camera{};
    camera.position = math::vec3{3, 3, 3};
    camera.target_position = math::vec3{0, 0, 0};
    camera.near = 0.1;
    camera.far = 100.;
    camera.fov = M_PI_2;
    camera.width = window.get_size().width;
    camera.height = window.get_size().height;

    window.register_mouse_scroll_callback([&camera](auto e) {
        auto dir = math::normalize(camera.target_position - camera.position);
        auto new_pos = camera.position + dir * e.y_offset;
        camera.position = new_pos;
    });

    window.register_mouse_click_handler([](::renderer::mouse_click_event e) {
        if (e.action == ::renderer::action_type::press && e.button == ::renderer::mouse_click_event::button_type::left) {
            mouse_clicked = true;
        }
        if (e.action == ::renderer::action_type::release && e.button == ::renderer::mouse_click_event::button_type::left) {
            mouse_clicked = false;
        }
    });

    window.register_mouse_position_handler([&q, &window, &mouse_position](::renderer::mouse_position_event e) {
        static float angle_x = 0;
        static float angle_y = 0;
        static auto last_x = 0;
        static auto last_y = 0;

        if (mouse_clicked) {
            angle_x += (last_y - e.y) * 0.01;
//            auto x = math::misc::lerp(0, M_PI, float(e.y / window.get_size().height));
            auto y = math::misc::lerp(0, M_PI * 2, float(e.x / window.get_size().width));

            auto v = math::vec3 {1, 1, 0};
            std::cout << "x " << v.x << " y " << v.y  << " z " << v.z << std::endl;
            q = math::normalize(math::quaternion_rotation(v, angle_x));
        }

        last_y = e.y;
        last_x = e.x;
        mouse_position = {float(e.x), float(e.y)};
    });

    auto r = window.get_renderer();

    renderer::parameters_list_descriptor shader_params = {
        .parameters = {
            renderer::parameter_type::mat4,
            renderer::parameter_type::mat4,
            renderer::parameter_type::mat4,
            renderer::parameter_type::mat4}
    };

    const auto instance_params = r->create_parameters_list(shader_params);

    misc::images_loader loader{};

    auto uv_map_texture = r->create_texture(loader.load_2d_texture("./resources/uv_grid.png"));

    renderer::shader_descriptor shader_descriptor {
        .stages = {{.name = renderer::shader_stage_name::vertex, .code = vs}, {.name = renderer::shader_stage_name::fragment, .code = fs}},
        .samplers = {
            {"s_uv_map", uv_map_texture}
        },
        .parameters = {{"instance_data", instance_params}},

        .state = {
            .color_write = true,
            .depth_write = true,
            .depth_test = renderer::depth_test_mode::less_eq,
            .cull = renderer::cull_mode::back,
        },
    };

    renderer::shader_descriptor normals_shader_descriptor {
        .stages = {
            {.name = renderer::shader_stage_name::vertex, .code = vs},
            {.name = renderer::shader_stage_name::fragment, .code = normal_fs},
            {.name = renderer::shader_stage_name::geometry, .code = normal_gs}},

        .parameters = {{"instance_data", instance_params}},

        .state = {
            .color_write = true,
            .depth_write = true,
            .depth_test = renderer::depth_test_mode::less_eq,
            .cull = renderer::cull_mode::off,
        },
    };

    const auto shader = r->create_shader(shader_descriptor);
    const auto normals_shader = r->create_shader(normals_shader_descriptor);

    shapes::sphere sphere{};
    shapes::cylinder cylinder{};

    renderer::mesh_layout_descriptor mesh_layout_descriptor {
        .vertex_attributes = {
            {.data_type = renderer::data_type::f32, .elements_count = 3},
            {.data_type = renderer::data_type::f32, .elements_count = 2},
            {.data_type = renderer::data_type::f32, .elements_count = 3},
            {.data_type = renderer::data_type::f32, .elements_count = 3}},
        .vertex_data = {reinterpret_cast<const uint8_t*>(vertices), reinterpret_cast<const uint8_t*>(vertices) + sizeof(vertices)},
        .topology = renderer::geometry_topology::triangles
    };

    cylinder.generate(
    mesh_layout_descriptor.vertex_data,
    mesh_layout_descriptor.index_data,
    14,
    shapes::shape::triangulate | shapes::shape::gen_uv | shapes::shape::gen_normal | shapes::shape::gen_tangents);

    const auto mesh = r->create_mesh(mesh_layout_descriptor);

    renderer::texture_descriptor color_attachment_tex_descriptor{
        .pixels_data_type = renderer::data_type::u8,
        .format = renderer::texture_format::rgba,
        .type = renderer::texture_type::attachment,
        .size = {
            1, 1, 0, 1}};

    renderer::texture_descriptor depth_attachment_tex_descriptor{
        .pixels_data_type = renderer::data_type::d24,
        .format = renderer::texture_format::rgba,
        .type = renderer::texture_type::attachment,
        .size = {
            1, 1, 0, 1}};

    auto color_attachment_tex = r->create_texture(color_attachment_tex_descriptor);
    auto depth_attachment_tex = r->create_texture(depth_attachment_tex_descriptor);

    renderer::pass_descriptor pass_descriptor{
        .width = 1600,
        .height = 1200,
        .attachments = {
            {.type = renderer::attachment_type::color, .render_texture = color_attachment_tex},
            {.type = renderer::attachment_type::depth, .render_texture = depth_attachment_tex},
        }};

    auto pass = r->create_pass(pass_descriptor);

    while (!window.closed()) {
        camera.update();
        auto m = math::to_matrix(q);
        auto normal_matrix = math::transpose(math::inverse(math::transpose(m)));
        auto mvp = math::transpose(camera.get_transformation() * m);
        auto m_transposed = math::transpose(m);
        auto vp = math::transpose(camera.get_proj() * camera.get_view());

        auto mouse_dir = math::screen_coords_to_world_space_vector(
            mouse_position,
            {800, 600},
            camera.position, camera.target_position,
            {0, 1, 0},
            camera.fov,
            camera.near,
            camera.far);

        math::raytracing::ray3 ray{camera.position, mouse_dir};
        auto intersect = math::raytracing::intersect(ray, bound);

        if (intersect) {
            std::cout << "intersect" << std::endl;
        }

        r->encode_draw_command({.type = renderer::draw_command_type::pass, .pass = pass});

        r->set_parameter_data(instance_params, 0, math::value_ptr(mvp));
        r->set_parameter_data(instance_params, 1, math::value_ptr(m_transposed));
        r->set_parameter_data(instance_params, 2, math::value_ptr(normal_matrix));
        r->set_parameter_data(instance_params, 3, math::value_ptr(vp));

        r->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = mesh, .shader = shader});
        r->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = mesh, .shader = normals_shader});
        window.update();
    }
}
