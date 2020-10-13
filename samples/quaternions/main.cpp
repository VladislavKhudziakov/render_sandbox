
#include <math/quaternion.hpp>
#include <math/misc/misc.hpp>
#include <math/bound_boxes/bound.hpp>

#include <window/glfw_window.hpp>
#include <renderer/renderer.hpp>
#include <renderer/camera.hpp>
#include <math/matrix_operations.hpp>
#include <math/raytracing/ray.hpp>

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
layout (std140) uniform instance_data
{
    mat4 mvp;
};

out vec4 v_color;

void main()
{
    gl_Position = mvp * vec4(attr_pos, 1.);
    v_color = vec4(float(gl_VertexID) / 36.0, 0.4, 0.6, 1.0);
})";

constexpr auto fs = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

in vec4 v_color;

void main()
{
    frag_color = v_color;
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
    camera.position = math::vec3{2, 3, 2};
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
        static float angle = 0;
        static auto last_x = 0;

        if (mouse_clicked) {
            auto t = e.x / window.get_size().width;
            q = math::normalize(math::quaternion_rotation(math::vec3 {1, 0, 1}, math::misc::lerp(0, M_PI, t)));
//            auto q2 = math::quaternion_rotation(math::normalize(math::vec3 {0, 1, 0}), M_PI_4);
//            q = math::slerp(q1, q2, t);
        }

        last_x = e.x;
        mouse_position = {float(e.x), float(e.y)};
    });

    auto r = window.get_renderer();

    renderer::parameters_list_descriptor shader_params = {
        .parameters = {renderer::parameter_type::mat4}
    };

    const auto instance_params = r->create_parameters_list(shader_params);

    renderer::shader_descriptor shader_descriptor {
        .stages = {{.name = renderer::shader_stage_name::vertex, .code = vs}, {.name = renderer::shader_stage_name::fragment, .code = fs}},
        .parameters = {{"instance_data", instance_params}},
        .state = {
            .color_write = true,
            .depth_write = true,
            .depth_test = renderer::depth_test_mode::less_eq,
            .cull = renderer::cull_mode::off,
        },
    };

    const auto shader = r->create_shader(shader_descriptor);

    renderer::mesh_layout_descriptor mesh_layout_descriptor {
        .vertex_attributes = {renderer::vertex_attribute{.data_type = renderer::data_type::f32, .elements_count = 3}},
        .vertex_data = {reinterpret_cast<const uint8_t*>(vertices), reinterpret_cast<const uint8_t*>(vertices) + sizeof(vertices)}
    };

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
        auto mvp = math::transpose(camera.get_transformation() * m);
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
        r->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = mesh, .shader = shader});
        window.update();
    }
}
