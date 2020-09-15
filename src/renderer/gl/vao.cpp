

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

    if (!vld.index_data.empty()) {
        ebo.emplace(vld.index_data.size());
        ebo_bind.emplace(ebo.value());
        ebo->load_data(reinterpret_cast<const void*>(vld.index_data.data()));
    }

    vbo.load_data(reinterpret_cast<const void*>(vld.vertex_data.data()));

    bind_guard this_bind(*this);

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
}


void renderer::gl::vao::draw()
{
    bind_guard bind(*this);

    if (m_indices_count > 0) {
        glDrawElements(GL_TRIANGLES, m_indices_count, m_indices_format, reinterpret_cast<void*>(0));
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertices_count);
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

    ASSERT(instances_count > 0);
    if (m_indices_count > 0) {
        glDrawElementsInstanced(GL_TRIANGLES, m_indices_count, m_indices_format, reinterpret_cast<void*>(0), instances_count);
    } else {
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertices_count, instances_count);
    }
}
