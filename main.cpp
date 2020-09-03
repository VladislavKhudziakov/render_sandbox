
#include <window/glfw_window.hpp>
#include <renderer/gl/renderer.hpp>
#include <renderer/renderer.hpp>

#include <glad/glad.h>

constexpr auto vss = R"(#version 410 core
layout (location = 0) in vec3 v_pos;

void main()
{
    gl_Position = vec4(v_pos.xy, 0., 1.);
}
)";

constexpr auto fss = R"(#version 410 core
layout (location = 0) out vec4 frag_color;

void main()
{
    frag_color = vec4(1., 1., 0., 1.);
}
)";

constexpr float arr[] {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f,  0.5f, 0.0f
};

int main() {
    renderer::gl::renderer r;

    renderer::mesh_handler mesh;
    renderer::shader_handler shader;

    renderer::glfw_window window("my window", {800, 600}, [&r, &mesh, &shader](float time) {
        r.update(time);
        r.draw(mesh, shader);
    });


    renderer::mesh_layout_descriptor md {
        .vertex_attributes = {
            {renderer::type::f32, 3}
        },
        .vertex_data = { reinterpret_cast<const uint8_t*>(arr),  reinterpret_cast<const uint8_t*>(arr) + sizeof(arr)}
    };

    renderer::shader_descriptor sd {
        .stages = {
            {renderer::shader_stage_name::vertex, vss},
            {renderer::shader_stage_name::fragment, fss}
        }
    };

    mesh = r.create_mesh(md);
    shader = r.create_shader(sd);

    while (!window.closed()) {
        window.update();
    }

    return 0;
}
