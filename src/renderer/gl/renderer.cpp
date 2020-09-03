

#include "renderer.hpp"

#include <renderer/gl/bind_guard.hpp>


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
    bind_guard shader_bind(m_shaders[shader_handler]);
    auto& shader_samplers = m_shaders[shader_handler].m_samplers;

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
