

#include "renderer.hpp"

#include <renderer/gl/bind_guard.hpp>


renderer::mesh_handler renderer::gl::renderer::create_mesh(const ::renderer::mesh_layout_descriptor& descriptor)
{
    m_vaos.emplace_back(descriptor);
    return m_vaos.size() - 1;
}


renderer::shader_handler renderer::gl::renderer::create_shader(const ::renderer::shader_descriptor& descriptor)
{
    m_shaders.emplace_back(descriptor);
    return m_shaders.size() - 1;
}


void renderer::gl::renderer::draw(
    ::renderer::mesh_handler mesh_handler,
    ::renderer::shader_handler shader_handler)
{
    bind_guard shader_bind(m_shaders[shader_handler]);
    m_vaos[mesh_handler].draw();
}


void renderer::gl::renderer::update(float time)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
