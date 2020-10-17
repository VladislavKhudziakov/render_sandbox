

#include "vao.hpp"

#include <renderer/renderer.hpp>
#include <renderer/gl/traits.hpp>
#include <renderer/gl/buffer.hpp>


renderer::gl::vao::vao(const ::renderer::mesh_layout_descriptor& vld)
{
    vertex_buffer vbo(vld.vertex_data.size());
    std::optional<element_buffer> ebo;

    bind_guard vbo_bind(vbo);
    std::optional<bind_guard<element_buffer>> ebo_bind;

    bind_guard this_bind(*this);

    if (!vld.index_data.empty()) {
        ebo.emplace(vld.index_data.size());
        ebo_bind.emplace(ebo.value());
        ebo->load_data(reinterpret_cast<const void*>(vld.index_data.data()));
    }

    vbo.load_data(reinterpret_cast<const void*>(vld.vertex_data.data()));

    auto vertex_attribs_size = vld.vertex_attributes.size();

    size_t stride = 0;
    size_t offset = 0;

    for (int i = 0; i < vertex_attribs_size; ++i) {
        const auto& attr = vld.vertex_attributes[i];
        auto gl_type = traits::get_gl_type(attr.data_type);
        stride += gl_type.type_size * attr.elements_count;
    }

    for (int i = 0; i < vertex_attribs_size; ++i) {
        const auto& attr = vld.vertex_attributes[i];
        auto gl_type = traits::get_gl_type(attr.data_type);
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attr.elements_count, gl_type.gl_format, GL_FALSE, stride, (void*) offset);
        offset += gl_type.type_size * attr.elements_count;
    }

    auto gl_type = traits::get_gl_type(vld.indices_data_type);

    m_vertices_count = vld.vertex_data.size() / stride;
    m_indices_count = vld.index_data.size() / gl_type.type_size;
    m_indices_format = gl_type.gl_format;
    m_geometry_topology = traits::get_gl_geom_topology(vld.topology, vld.adjacent);
}


void renderer::gl::vao::draw()
{
    bind_guard bind(*this);

    if (m_geometry_topology == GL_POINTS) {
        glEnable(GL_PROGRAM_POINT_SIZE);
    }

    if (m_indices_count > 0) {
        glDrawElements(m_geometry_topology, m_indices_count, m_indices_format, reinterpret_cast<void*>(0));
    } else {
        glDrawArrays(m_geometry_topology, 0, m_vertices_count);
    }

    if (m_geometry_topology == GL_POINTS) {
        glDisable(GL_PROGRAM_POINT_SIZE);
    }
}

void renderer::gl::vao::bind()
{
    glBindVertexArray(m_handler);
}

void renderer::gl::vao::unbind()
{
    glBindVertexArray(0);
}


void renderer::gl::vao::draw_instanced(uint32_t instances_count)
{
    bind_guard bind(*this);

    if (m_geometry_topology == GL_POINTS) {
        glEnable(GL_PROGRAM_POINT_SIZE);
    }

    ASSERT(instances_count > 0);
    if (m_indices_count > 0) {
        glDrawElementsInstanced(m_geometry_topology, m_indices_count, m_indices_format, reinterpret_cast<void*>(0), instances_count);
    } else {
        glDrawArraysInstanced(m_geometry_topology, 0, m_vertices_count, instances_count);
    }

    if (m_geometry_topology == GL_POINTS) {
        glDisable(GL_PROGRAM_POINT_SIZE);
    }
}
