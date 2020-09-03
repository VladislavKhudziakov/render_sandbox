#include <renderer/renderer.hpp>
#include <renderer/gl/vao.hpp>
#include <renderer/gl/shader.hpp>

#pragma once

namespace renderer::gl
{
    class renderer : public ::renderer::renderer
    {
    public:
        mesh_handler create_mesh(const mesh_layout_descriptor& descriptor) override;
        shader_handler create_shader(const shader_descriptor& descriptor) override;
        void draw(mesh_handler mesh_handler, shader_handler shader_handler) override;
        void update(float) override;

    private:
        std::vector<vao> m_vaos;
        std::vector<shader> m_shaders;
    };
} // namespace renderer::gl
