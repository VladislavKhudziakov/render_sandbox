#include <renderer/renderer.hpp>
#include <renderer/gl/vao.hpp>
#include <renderer/gl/shader.hpp>
#include <renderer/gl/texture.hpp>
#include <renderer/gl/parameters_list.hpp>

#pragma once

namespace renderer::gl
{
    class renderer : public ::renderer::renderer
    {
    public:
        mesh_handler create_mesh(const mesh_layout_descriptor& descriptor) override;
        void destroy_mesh(mesh_handler handler) override;


        shader_handler create_shader(const shader_descriptor& descriptor) override;
        void set_shader_sampler(shader_handler, texture_handler, const std::string&) override;
        void destroy_shader(shader_handler handler) override;

        texture_handler create_texture(const texture_descriptor& descriptor) override;
        void destroy_texture(texture_handler handler) override;


        ::renderer::parameters_list_handler create_parameters_list(const parameters_list_descriptor& descriptor) override;
        void set_parameter_data(::renderer::parameters_list_handler, uint32_t parameter_index, void* data) override;
        void destroy_parameters_list(parameters_list_handler handler) override;

        void draw(mesh_handler mesh_handler, shader_handler shader_handler) override;

        void encode_draw_command(draw_command command) override;

        void update(float) override;

    private:
        std::vector<vao> m_vaos;
        std::vector<shader> m_shaders;
        std::vector<texture> m_textures;
        std::vector<parameters_list> m_params_lists;
        std::vector<::renderer::draw_command> m_commands_buffer;
    };
} // namespace renderer::gl
