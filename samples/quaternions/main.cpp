
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
#include <shapes/curve.hpp>

#include <scene/mesh_instance.hpp>
#include <misc/opengl.hpp>
#include <misc/types_traits.hpp>

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
    mat4 mvp[3];
    mat4 model_matrix[3];
    mat4 normal_matrix[3];
    mat4 vp_matrix[3];
    mat4 light_space_matrix;
};

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec3 v_pos;
out vec4 v_light_space_pos;

uniform int DrawID;

void main()
{
    gl_Position = mvp[DrawID] * vec4(attr_pos, 1.);
    v_normal = mat3(normal_matrix[DrawID]) * attr_normal;
    v_tangent = mat3(normal_matrix[DrawID]) * attr_tangent;
    v_bitangent = mat3(normal_matrix[DrawID]) * cross(attr_normal, attr_tangent);
    v_pos = vec3(model_matrix[DrawID] * vec4(attr_pos, 1.));
    v_uv = attr_uv;
    v_uv.y = 1. - v_uv.y;
    v_light_space_pos = light_space_matrix * model_matrix[DrawID] * vec4(attr_pos, 1.);
})";

constexpr auto shadow_debug_vs = R"(#version 410 core

layout (location = 0) in vec3 attr_pos;


out vec2 var_uv;

void main()
{
    gl_Position = vec4(attr_pos.xy, 0., 1.);
    var_uv = attr_pos.xy * 0.5 + 0.5;
})";


constexpr auto shadow_debug_fs = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

in vec2 var_uv;

uniform sampler2D s_shadow_tex;

float LinearizeDepth(float depth, float near_plane, float far_plane)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float z = texture(s_shadow_tex, var_uv).r;
    frag_color = vec4(z, z, z, 1.);
})";

constexpr auto post_process_fs = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

in vec2 var_uv;

uniform sampler2D s_src_tex;

void main()
{
    vec4 c = texture(s_src_tex, var_uv);
    frag_color = c;
})";

constexpr auto shadow_vs = R"(#version 410 core

layout (location = 0) in vec3 attr_pos;

layout (std140) uniform instance_data
{
    mat4 mvp[3];
};

uniform int DrawID;

void main()
{
    gl_Position = mvp[DrawID] * vec4(attr_pos, 1.);
})";


constexpr auto shadow_fs = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

void main()
{
})";

constexpr auto normal_gs = R"(#version 410 core
layout(triangles) in;
layout(line_strip, max_vertices = 36) out;

layout (std140) uniform instance_data
{
    mat4 mvp[3];
    mat4 model_matrix[3];
    mat4 normal_matrix[3];
    mat4 vp_matrix[3];
    mat4 light_space_matrix;
};

in vec2 v_uv[3];
in vec3 v_normal[3];
in vec3 v_tangent[3];
in vec3 v_bitangent[3];
in vec3 v_pos[3];

out vec3 g_color;

uniform int DrawID;

void emit_line(vec3 color, vec3 start_pos, vec3 end_pos)
{
    g_color = color;
    gl_Position = vp_matrix[DrawID] * vec4(start_pos, 1.);
    EmitVertex();
    g_color = color;
    gl_Position = vp_matrix[DrawID] * vec4(end_pos, 1.);
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
in vec3 v_bitangent;
in vec3 v_pos;
in vec4 v_light_space_pos;

uniform sampler2D s_uv_map;
uniform sampler2D s_shadow_map;
uniform sampler2D s_test;

vec3 light_dir = normalize(vec3(3, 6, -6) - vec3(0, 0, 0));

void main()
{
    vec3 light_space_pos = v_light_space_pos.xyz / v_light_space_pos.w;
    light_space_pos = light_space_pos * 0.5 + 0.5;
    vec2 shadow_uv = light_space_pos.xy;
    float shadow_z = texture(s_shadow_map, shadow_uv).r;

    vec4 tex_color = texture(s_uv_map, v_uv);
    vec4 test_color = texture(s_test, v_uv);

    float bias = max(0.05 * (1.0 - dot(v_normal, light_dir)), 0.01);

    if (light_space_pos.z <= 1.0f && light_space_pos.z - bias > shadow_z) {
        frag_color = tex_color * 0.1;
        return;
    }

    float c = max(dot(v_normal, light_dir), 0.0);

    frag_color = tex_color * test_color * c;
}
)";

bool mouse_clicked = false;

int main()
{
    math::quaternion q = math::quaternion_rotation({0, 0, 1}, 0);
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
            q = math::normalize(math::quaternion_rotation(v, angle_x));
        }

        last_y = e.y;
        last_x = e.x;
        mouse_position = {float(e.x), float(e.y)};
    });

    auto r = window.get_renderer();

    shapes::sphere sphere{50, shapes::procedural::triangulate | shapes::procedural::gen_uv | shapes::procedural::gen_normal | shapes::procedural::gen_tangents};
    shapes::cylinder cylinder{50, shapes::procedural::triangulate | shapes::procedural::gen_uv | shapes::procedural::gen_normal | shapes::procedural::gen_tangents};
    shapes::curve curve{50, shapes::procedural::triangulate | shapes::procedural::gen_uv | shapes::procedural::gen_normal | shapes::procedural::gen_tangents};

    std::vector<std::unique_ptr<renderer::scene::mesh_instance>> mesh_instances_list {};
    mesh_instances_list.emplace_back(std::make_unique<renderer::scene::mesh_instance>(r, std::make_unique<shapes::sphere>(50, shapes::procedural::triangulate | shapes::procedural::gen_uv | shapes::procedural::gen_normal | shapes::procedural::gen_tangents)));
    mesh_instances_list.emplace_back(std::make_unique<renderer::scene::mesh_instance>(r, std::make_unique<shapes::cylinder>(
        50, shapes::procedural::triangulate | shapes::procedural::gen_uv | shapes::procedural::gen_normal | shapes::procedural::gen_tangents)));
    mesh_instances_list.emplace_back(std::make_unique<renderer::scene::mesh_instance>(r, std::make_unique<shapes::curve>(
        50,
        shapes::procedural::triangulate | shapes::procedural::gen_uv | shapes::procedural::gen_normal | shapes::procedural::gen_tangents,
        math::vec3 {0.0f, 0.0f, -5.f},
        math::vec3 {0.f, 1.f * 2, (-1.f / 3.f) * 10.f},
        math::vec3 {0.f, -1.f * 2, (1.f / 3.f) * 10.f},
        math::vec3 {0.f, 0.f, 5.f},
        10)));

    mesh_instances_list[0]->local_transform =  math::translation({0, 2, 0}) * math::scale({0.4, 0.4, 0.4});
    mesh_instances_list[1]->local_transform = math::translation({0.5, 3, -3.5})  * math::scale({0.5, 5, 0.5}) * math::rotation_x(M_PI_2);

    renderer::parameters_list_descriptor shader_params = {};
    renderer::parameters_list_descriptor shadow_params_descriptor = {};

    shadow_params_descriptor.parameters.reserve(mesh_instances_list.size());
    shader_params.parameters.reserve(mesh_instances_list.size() * 4 + 1);

    for (const auto& mesh : mesh_instances_list) {
        shader_params.parameters.emplace_back(renderer::parameter_type::mat4);
        shader_params.parameters.emplace_back(renderer::parameter_type::mat4);
        shader_params.parameters.emplace_back(renderer::parameter_type::mat4);
        shader_params.parameters.emplace_back(renderer::parameter_type::mat4);

        shadow_params_descriptor.parameters.emplace_back(renderer::parameter_type::mat4);
    }
    shader_params.parameters.emplace_back(renderer::parameter_type::mat4);

    constexpr float quad_verts[] = {
        -1, -1, 0,
        1, -1, 0,
        1, 1, 1,
        -1, -1, 0,
        -1, 1, 0,
        1, 1, 1
    };

    renderer::mesh_layout_descriptor shadow_debug_mld = {
        .vertex_attributes = {{.data_type = renderer::data_type::f32, .elements_count = 3}},
        .vertex_data = {reinterpret_cast<const uint8_t*>(quad_verts), reinterpret_cast<const uint8_t*>(quad_verts) + sizeof(quad_verts)},
    };

    const auto shadow_debug_mesh = r->create_mesh(shadow_debug_mld);

    const auto instance_params = r->create_parameters_list(shader_params);
    const auto shadow_params = r->create_parameters_list(shadow_params_descriptor);

    misc::images_loader loader{};

    auto uv_map_texture = r->create_texture(loader.load_2d_texture("./resources/uv_grid.png"));
    auto test_texture = r->create_texture(loader.load_2d_texture("./resources/mlg.png"));

    renderer::texture_descriptor draw_color_attachment_tex_descriptor{
        .pixels_data_type = renderer::data_type::u8,
        .format = renderer::texture_format::rgba,
        .type = renderer::texture_type::attachment,
        .size = {
            1600, 1200, 0, 1}};

    renderer::texture_descriptor draw_depth_attachment_tex_descriptor{
        .pixels_data_type = renderer::data_type::d24,
        .format = renderer::texture_format::rgba,
        .type = renderer::texture_type::attachment,
        .size = {
            1600, 1200, 0, 1}};

    renderer::texture_descriptor post_process_color_tex_descriptor{
        .pixels_data_type = renderer::data_type::u8,
        .format = renderer::texture_format::rgba,
        .type = renderer::texture_type::attachment,
        .size = {
            1, 1, 0, 1}};

    renderer::texture_descriptor shadow_depth_tex_descriptor{
        .pixels_data_type = renderer::data_type::d24,
        .format = renderer::texture_format::r,
        .type = renderer::texture_type::attachment,
        .size = {
            1, 1, 0, 1}};

    auto shadow_depth_attachment_tex = r->create_texture(shadow_depth_tex_descriptor);
    auto post_process_color_attachment_tex = r->create_texture(post_process_color_tex_descriptor);

    auto draw_color_attachment_tex = r->create_texture(draw_color_attachment_tex_descriptor);
    auto draw_depth_attachment_tex = r->create_texture(draw_depth_attachment_tex_descriptor);


    renderer::pass_descriptor draw_pass_descriptor{
        .width = 1600,
        .height = 1200,
        .attachments = {
            {.type = renderer::attachment_type::color, .render_texture = draw_color_attachment_tex},
            {.type = renderer::attachment_type::depth, .render_texture = draw_depth_attachment_tex},
        },
        .state = {
            .start = renderer::pass_start_behavior::clear,
            .finish = renderer::pass_finish_behavior::store,
            .clear_color = {1, 1, 1, 1},
            .clear_depth = 1
        }
    };

    renderer::pass_descriptor shadow_pass_descriptor{
        .width = 400,
        .height = 300,
        .attachments = {
            {.type = renderer::attachment_type::depth, .render_texture = shadow_depth_attachment_tex},
        },
        .state = {
            .start = renderer::pass_start_behavior::clear,
            .finish = renderer::pass_finish_behavior::store,
            .clear_color = {1, 1, 1, 1},
            .clear_depth = 1,
        }
    };

    renderer::pass_descriptor post_process_pass_descriptor{
        .width = 1600,
        .height = 1200,
        .attachments = {
            {.type = renderer::attachment_type::color, .render_texture = post_process_color_attachment_tex},
        },
        .state = {
            .start = renderer::pass_start_behavior::clear,
            .finish = renderer::pass_finish_behavior::store,
            .clear_color = {1, 1, 1, 1},
            .clear_depth = 1
        }
    };

    auto shadow_pass = r->create_pass(shadow_pass_descriptor);
    auto draw_pass = r->create_pass(draw_pass_descriptor);
    auto post_process_pass = r->create_pass(post_process_pass_descriptor);

    renderer::shader_descriptor shader_descriptor {
        .stages = {{.name = renderer::shader_stage_name::vertex, .code = vs}, {.name = renderer::shader_stage_name::fragment, .code = fs}},
        .samplers = {
            {"s_uv_map", uv_map_texture},
            {"s_shadow_map", shadow_depth_attachment_tex},
            {"s_test", test_texture}
        },
        .parameters = {{"instance_data", instance_params}},

        .state = {
            .color_write = true,
            .depth_write = true,
            .depth_test = renderer::depth_test_mode::less_eq,
            .cull = renderer::cull_mode::off,
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

    renderer::shader_descriptor shadow_debug_shader_descriptor {
        .stages = {{.name = renderer::shader_stage_name::vertex, .code = shadow_debug_vs}, {.name = renderer::shader_stage_name::fragment, .code = shadow_debug_fs}},
        .samplers = {
            {"s_shadow_tex", shadow_depth_attachment_tex}
        },
        .state = {
            .color_write = true,
            .depth_write = false,
            .depth_test = renderer::depth_test_mode::off,
            .cull = renderer::cull_mode::off,
        },
    };

    renderer::shader_descriptor post_process_shader_descriptor {
        .stages = {{.name = renderer::shader_stage_name::vertex, .code = shadow_debug_vs}, {.name = renderer::shader_stage_name::fragment, .code = post_process_fs}},
        .samplers = {
            {"s_src_tex", draw_color_attachment_tex}
        },
        .state = {
            .color_write = true,
            .depth_write = false,
            .depth_test = renderer::depth_test_mode::off,
            .cull = renderer::cull_mode::off,
        },
    };

    renderer::shader_descriptor shadow_shader_descriptor {
        .stages = {{.name = renderer::shader_stage_name::vertex, .code = shadow_vs}, {.name = renderer::shader_stage_name::fragment, .code = shadow_fs}},
        .parameters = {{"instance_data", shadow_params}},
        .state = {
            .color_write = true,
            .depth_write = true,
            .depth_test = renderer::depth_test_mode::less_eq,
            .cull = renderer::cull_mode::off,
        },
    };

    const auto shader = r->create_shader(shader_descriptor);
    const auto normals_shader = r->create_shader(normals_shader_descriptor);
    const auto shadow_shader = r->create_shader(shadow_shader_descriptor);
    const auto shadow_debug_shader = r->create_shader(shadow_debug_shader_descriptor);
    const auto post_process_shader = r->create_shader(post_process_shader_descriptor);

    auto light_proj = math::ortho(20, 20, 0.01, 20);
    auto light_view = math::look_at({0, 18, -6}, {0, 0, 0}, {0, 1, 0});
    auto light_vp = light_proj * light_view;
    auto light_vp_transposed = math::transpose(light_vp);

    while (!window.closed()) {
        camera.update();
        auto global_transform = math::to_matrix(q);

        r->encode_draw_command({.type = renderer::draw_command_type::pass, .pass = shadow_pass});

        for (uint32_t i = 0; i < mesh_instances_list.size(); ++i) {
            const auto& mesh_instance = mesh_instances_list[i];
            auto m = global_transform * mesh_instance->local_transform;
            auto mvp = math::transpose(light_proj * light_view * m);

            r->set_parameter_data(shadow_params, i, math::value_ptr(mvp));
            r->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = mesh_instance->get_handler(), .shader = shadow_shader, .draw_id = i});
        }

        r->encode_draw_command({.type = renderer::draw_command_type::pass, .pass = draw_pass});
        r->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = shadow_debug_mesh, .shader = shadow_debug_shader});
        r->set_parameter_data(instance_params, shader_params.parameters.size() - 1, math::value_ptr(light_vp_transposed));

        for (uint32_t i = 0; i < mesh_instances_list.size(); ++i) {
            const auto& mesh_instance = mesh_instances_list[i];
            auto m = global_transform * mesh_instance->local_transform;
            auto normal_matrix = math::transpose(math::inverse(math::transpose(m)));
            auto m_transposed = math::transpose(m);

            auto mvp = math::transpose(camera.get_transformation() * m);
            auto vp = math::transpose(camera.get_proj() * camera.get_view());

            r->set_parameter_data(instance_params, i, math::value_ptr(mvp));
            r->set_parameter_data(instance_params, i + mesh_instances_list.size(), math::value_ptr(m_transposed));
            r->set_parameter_data(instance_params, i + mesh_instances_list.size() * 2, math::value_ptr(normal_matrix));
            r->set_parameter_data(instance_params, i + mesh_instances_list.size() * 3, math::value_ptr(vp));

            r->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = mesh_instance->get_handler(), .shader = shader, .draw_id = i});
        }

        r->encode_draw_command({.type = renderer::draw_command_type::pass, .pass = post_process_pass});
        r->encode_draw_command({.type = renderer::draw_command_type::draw, .mesh = shadow_debug_mesh, .shader = post_process_shader});

        window.update();
    }
}
