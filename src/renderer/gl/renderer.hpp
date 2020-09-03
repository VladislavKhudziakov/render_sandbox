#include <renderer/renderer.hpp>
#include <renderer/gl/vao.hpp>
#include <renderer/gl/shader.hpp>
#include <renderer/gl/texture.hpp>


#pragma once

namespace renderer::gl
{
    class renderer : public ::renderer::renderer
    {
    public:
        mesh_handler create_mesh(const mesh_layout_descriptor& descriptor) override;
        shader_handler create_shader(const shader_descriptor& descriptor) override;
        texture_handler create_texture(const texture_descriptor& descriptor) override;
        void set_shader_sampler(shader_handler, texture_handler, const std::string&) override;
        void draw(mesh_handler mesh_handler, shader_handler shader_handler) override;
        void update(float) override;

    private:
        std::vector<vao> m_vaos;
        std::vector<shader> m_shaders;
        std::vector<texture> m_textures;
    };
} // namespace renderer::gl
