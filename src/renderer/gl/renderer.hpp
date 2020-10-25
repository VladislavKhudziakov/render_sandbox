
#pragma once


#include <renderer/renderer.hpp>
#include <renderer/gl/vao.hpp>
#include <renderer/gl/shader.hpp>
#include <renderer/gl/texture.hpp>
#include <renderer/gl/parameters_list.hpp>
#include <renderer/gl/render_pass.hpp>


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
        void load_texture_data(texture_handler handler, texture_size size, void* pVoid) override;

        ::renderer::parameters_list_handler create_parameters_list(const parameters_list_descriptor& descriptor) override;
        void set_parameter_data(::renderer::parameters_list_handler, uint32_t parameter_index, void* data) override;
        void destroy_parameters_list(parameters_list_handler handler) override;

        pass_handler create_pass(const pass_descriptor& descriptor) override;
        void destroy_pass(pass_handler handler) override;
        void resize_pass(pass_handler handler, size_t w, size_t h) override;

        void encode_draw_command(draw_command command) override;

        void update(float) override;

    private:
        void draw(mesh_handler mesh_handler, shader_handler shader_handler, uint32_t instances_count = 1, uint32_t draw_id = 0);
        void set_gpu_state(const ::renderer::shader_state&);

        std::vector<vao> m_vaos;
        std::vector<shader> m_shaders;
        std::vector<texture> m_textures;
        std::vector<parameters_list> m_params_lists;
        std::vector<render_pass> m_passes;

        std::vector<::renderer::draw_command> m_commands_buffer;
    };
} // namespace renderer::gl
