

#include "renderer.hpp"

renderer::mesh_handler renderer::gl::renderer::create_mesh(
    const ::renderer::mesh_layout_descriptor& descriptor)
{
    m_vaos.emplace_back(descriptor);
    return m_vaos.size() - 1;
}


renderer::shader_handler renderer::gl::renderer::create_shader(
    const ::renderer::shader_descriptor& descriptor)
{
    m_shaders.emplace_back(descriptor);
    return m_shaders.size() - 1;
}


void renderer::gl::renderer::draw(
    ::renderer::mesh_handler mesh_handler,
    ::renderer::shader_handler shader_handler)
{
    auto& shader = m_shaders[shader_handler];

    bind_guard shader_bind(shader);
    auto& shader_samplers = shader.m_samplers;

    uint32_t sampler_index = 0;

    for (const auto& [sampler_name, texture_index] : shader_samplers) {
        glActiveTexture(GL_TEXTURE0 + sampler_index++);
        m_textures[texture_index].bind();
    }

    m_vaos[mesh_handler].draw();
}


void renderer::gl::renderer::update(float time)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto& params_list : m_params_lists) {
        params_list.load_data_to_gpu();
    }

    for (const auto& command : m_commands_buffer) {
        switch (command.type) {
            case draw_command_type::pass:
                break;
            case draw_command_type::draw:
                draw(command.mesh, command.shader);
                break;
        }
    }

    m_commands_buffer.clear();
}


renderer::texture_handler renderer::gl::renderer::create_texture(
    const ::renderer::texture_descriptor& descriptor)
{
    m_textures.emplace_back(descriptor);
    return m_textures.size() - 1;
}


void renderer::gl::renderer::set_shader_sampler(
    ::renderer::shader_handler shader_handler,
    ::renderer::texture_handler texture_handler,
    const std::string& sampler_name)
{
    auto& shader = m_shaders[shader_handler];
    shader.m_samplers[sampler_name] = texture_handler;
}


::renderer::parameters_list_handler renderer::gl::renderer::create_parameters_list(const ::renderer::parameters_list_descriptor& descriptor)
{
    auto& params_list = m_params_lists.emplace_back(descriptor);

    const auto binding_index = m_params_lists.size() - 1;
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_index, params_list.m_gpu_storage->m_handler);

    return binding_index;
}


void renderer::gl::renderer::set_parameter_data(::renderer::parameters_list_handler handler, uint32_t param_index, void* data)
{
    m_params_lists[handler].set_parameter_data(param_index, data);
}


void renderer::gl::renderer::destroy_mesh(::renderer::mesh_handler handler)
{
    m_vaos.erase(m_vaos.begin() + handler);
}


void renderer::gl::renderer::destroy_shader(::renderer::shader_handler handler)
{
    m_shaders.erase(m_shaders.begin() + handler);
}


void renderer::gl::renderer::destroy_texture(::renderer::texture_handler handler)
{
    m_textures.erase(m_textures.begin() + handler);
}


void renderer::gl::renderer::destroy_parameters_list(::renderer::parameters_list_handler handler)
{
    m_params_lists.erase(m_params_lists.begin() + handler);
}


void renderer::gl::renderer::encode_draw_command(::renderer::draw_command command)
{
    m_commands_buffer.emplace_back(command);
}
