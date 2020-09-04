

#include "render_pass.hpp"

#include <renderer/renderer.hpp>
#include <misc/misc.hpp>


renderer::gl::render_pass::render_pass(const pass_descriptor& descriptor, std::vector<gl::texture>& textures)
    : m_width(descriptor.width)
    , m_height(descriptor.height)
    , m_attachments_list(descriptor.attachments)
{
    GLint old_fb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_fb);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer_handler);

    std::vector<GLenum> draw_buffers;
    draw_buffers.reserve(descriptor.attachments.size());

    size_t draw_buf_index = 0;
    for (const auto& attachment : descriptor.attachments) {
        auto& tex = textures[attachment.render_texture];

        ASSERT(tex.m_type == ::renderer::texture_type::attachment);

        tex.resize({descriptor.width, descriptor.height});

        switch (attachment.type) {
            case attachment_type::color:
                draw_buffers.emplace_back(GL_COLOR_ATTACHMENT0 + draw_buf_index);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + draw_buf_index, GL_TEXTURE_2D, tex.m_handler, 0);
                draw_buf_index++;
                break;
            case attachment_type::depth:
                draw_buffers.emplace_back(GL_COLOR_ATTACHMENT0 + draw_buf_index);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex.m_handler, 0);
                break;
        }
    }

    glDrawBuffers(draw_buffers.size(), draw_buffers.data());

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, old_fb);
}


void renderer::gl::render_pass::begin()
{
    glViewport(0, 0, m_width, m_height);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer_handler);

    if (m_state.start == ::renderer::pass_start_behavior::clear) {
        clear_pass();
    }
}


void renderer::gl::render_pass::end()
{
    if (m_state.finish == ::renderer::pass_finish_behavior::discard) {
        clear_pass();
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


void renderer::gl::render_pass::clear_pass()
{
    size_t color_buffer_index = 0;
    for (const auto& attachment : m_attachments_list) {
        switch (attachment.type) {
            case attachment_type::color:
                glClearBufferfv(GL_COLOR, color_buffer_index++, m_state.clear_color);
                break;
            case attachment_type::depth:
                glClearBufferfv(GL_DEPTH, 0, &m_state.clear_depth);
                break;
        }
    }
}


void renderer::gl::render_pass::resize(size_t width, size_t height, std::vector<gl::texture>& textures_list)
{
    if (m_width != width || m_height != height) {
        for (const auto& attachment : m_attachments_list) {
            auto& texture = textures_list[attachment.render_texture];
            ASSERT(texture.m_type == ::renderer::texture_type::attachment);
            texture.resize({width, height});
        }
    }

    m_width = width;
    m_height = height;
}
