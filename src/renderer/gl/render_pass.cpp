

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

    if (descriptor.state.msaa > 1) {
        m_msaa_resolve_framebuffer.emplace();
    }

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
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex.m_handler, 0);
                break;
        }
    }

    glDrawBuffers(draw_buffers.size(), draw_buffers.data());

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *m_msaa_resolve_framebuffer);

    m_renderbuffers.reserve(descriptor.attachments.size());

    if (!m_msaa_resolve_framebuffer.has_value()) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, old_fb);
        return;
    }

    draw_buf_index = 0;
    draw_buffers.clear();


    for (const auto& attachment : descriptor.attachments) {
        auto& tex = textures[attachment.render_texture];
        const auto [type, int_fmt, fmt] = tex.m_storage_data;
        auto& curr_rb = m_renderbuffers.emplace_back(descriptor.state.msaa, fmt, descriptor.width, descriptor.height);
        switch (attachment.type) {
            case attachment_type::color: {
                draw_buffers.emplace_back(GL_COLOR_ATTACHMENT0 + draw_buf_index);
                glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + draw_buf_index, GL_RENDERBUFFER, curr_rb.m_handler);
                draw_buf_index++;
                break;
            }
            case attachment_type::depth: {
                glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, curr_rb.m_handler);
                break;
            }
        }
    }

    glDrawBuffers(draw_buffers.size(), draw_buffers.data());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, old_fb);
}


void renderer::gl::render_pass::begin()
{
    glViewport(0, 0, m_width, m_height);

    if (m_msaa_resolve_framebuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *m_msaa_resolve_framebuffer);
    } else {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer_handler);
    }

    if (m_state.start == ::renderer::pass_start_behavior::clear) {
        clear_pass();
    }
}


void renderer::gl::render_pass::end()
{
    if (m_state.finish == ::renderer::pass_finish_behavior::discard) {
        clear_pass();
    }

    if (m_msaa_resolve_framebuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer_handler);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, *m_msaa_resolve_framebuffer);
        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
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

        for (auto& renderbuffer : m_renderbuffers) {
            renderbuffer.resize(width, height);
        }
    }

    m_width = width;
    m_height = height;
}


renderer::gl::detail::renderbuffer::renderbuffer(uint32_t msaa, GLenum format, size_t width, size_t height)
    : m_samples_count(msaa)
    , m_format(format)
{
    bind();
    resize(width, height);
    unbind();
}


void renderer::gl::detail::renderbuffer::bind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, m_handler);
}


void renderer::gl::detail::renderbuffer::unbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


void renderer::gl::detail::renderbuffer::resize(size_t width, size_t height)
{
    if (m_samples_count > 1) {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_samples_count, m_format, width, height);
    } else {
        glRenderbufferStorage(GL_RENDERBUFFER, m_format, width, height);
    }
}
