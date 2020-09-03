

#include "vao.hpp"

#include <renderer/renderer.hpp>
#include <renderer/gl/traits.hpp>
#include <renderer/gl/buffer.hpp>
#include <renderer/gl/bind_guard.hpp>

#define COMBINE1(X,Y) X##Y
#define COMBINE(X,Y) COMBINE1(X,Y)
#define CHECK_ERR() auto COMBINE(err, __LINE__) = glGetError(); assert( COMBINE(err, __LINE__) == GL_NO_ERROR)


renderer::gl::vao::vao(const ::renderer::mesh_layout_descriptor& vld)
{
    vertex_buffer vbo;
    element_buffer ebo;

    bind_guard vbo_bind(vbo);
    bind_guard ebo_bind(ebo);

    ebo_bind.unbind();

    if (!vld.index_data.empty()) {
        ebo_bind.bind();
        ebo.load_data(reinterpret_cast<const void*>(vld.index_data.data()), vld.index_data.size());
    }

    vbo.load_data(reinterpret_cast<const void*>(vld.vertex_data.data()), vld.vertex_data.size());

    bind_guard this_bind(*this);

    auto vertex_attribs_size = vld.vertex_attributes.size();

    size_t stride = 0;
    size_t offset = 0;

    for (int i = 0; i < vertex_attribs_size; ++i) {
        const auto& attr = vld.vertex_attributes[i];
        auto gl_type = traits::get_gl_type(attr.type);
        stride += gl_type.type_size * attr.elements_count;
    }

    for (int i = 0; i < vertex_attribs_size; ++i) {
        const auto& attr = vld.vertex_attributes[i];
        auto gl_type = traits::get_gl_type(attr.type);
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attr.elements_count, gl_type.gl_format, GL_FALSE, stride, (void*)offset);
        offset += gl_type.type_size * attr.elements_count;
    }

    auto gl_type = traits::get_gl_type(vld.index_type);

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
