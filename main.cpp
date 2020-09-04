
#include <window/glfw_window.hpp>
#include <renderer/gl/renderer.hpp>
#include <renderer/renderer.hpp>
#include <misc/images_loader.hpp>

#include <glad/glad.h>

constexpr auto vss = R"(#version 410 core
layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec2 attr_uv;

layout (std140) uniform instance_data
{
    vec4 color;
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

constexpr float arr[] {
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.5f, 1.0f,
    0.0f,  0.5f, 0.0f, 1.0f, 0.0f
};

int main() {
    renderer::gl::renderer r;
    misc::images_loader loader;

    renderer::mesh_handler mesh;
    renderer::shader_handler shader;

    renderer::glfw_window window("my window", {800, 600}, [&r, &mesh, &shader](float time) {

        r.encode_draw_command({
            .type = renderer::draw_command_type::draw,
            .mesh = mesh,
            .shader = shader
        });

        r.update(time);
    });


    renderer::mesh_layout_descriptor md {
        .vertex_attributes = {
            {renderer::type::f32, 3},
            {renderer::type::f32, 2}
        },
        .vertex_data = { reinterpret_cast<const uint8_t*>(arr),  reinterpret_cast<const uint8_t*>(arr) + sizeof(arr)}
    };

    auto image = r.create_texture(loader.load_2d_texture("/Users/vladislavkhudiakov/Downloads/test_image.png"));

    renderer::parameters_list_descriptor parameters_list
    {
        .parameters = {renderer::parameter_type::vec4}
    };

    auto params_list = r.create_parameters_list(parameters_list);

    renderer::shader_descriptor sd {
        .stages = {
            {renderer::shader_stage_name::vertex, vss},
            {renderer::shader_stage_name::fragment, fss}
        },
        .samplers = {
            {"s_tex", image}
        },
        .parameters = {
            {"instance_data", params_list}
        }
    };

    mesh = r.create_mesh(md);
    shader = r.create_shader(sd);

    float color[] = {1, 0, 1, 1};
    r.set_parameter_data(params_list, 0, color);

    while (!window.closed()) {
        window.update();
    }

    return 0;
}
