

#pragma once

#include <renderer/gl/raii_storage.hpp>
#include <renderer/gl/texture.hpp>
#include <renderer/renderer.hpp>
#include <memory/pool_factory.hpp>

#include <cinttypes>
#include <glad/glad.h>

namespace renderer
{
    class pass_descriptor;
}

namespace renderer::gl
{
    class render_pass;

    namespace detail
    {
        struct framebuffer_create_policy
        {
            static void create(uint32_t* handler)
            {
                return glGenFramebuffers(1, handler);
            }
        };

        struct framebuffer_destroy_policy
        {
            static void destroy(uint32_t* handler)
            {
                return glDeleteFramebuffers(1, handler);
            }
        };

        using framebuffer_handler = raii_storage<framebuffer_create_policy, framebuffer_destroy_policy>;

        struct renderbuffer_create_policy
        {
            static void create(uint32_t* handler)
            {
                return glGenRenderbuffers(1, handler);
            }
        };

        struct renderbuffer_destroy_policy
        {
            static void destroy(uint32_t* handler)
            {
                return glDeleteRenderbuffers(1, handler);
            }
        };

        using renderbuffer_handler = raii_storage<renderbuffer_create_policy, renderbuffer_destroy_policy>;

        class renderbuffer
        {
            friend class ::renderer::gl::render_pass;

        public:
            renderbuffer(uint32_t msaa, GLenum format, size_t width = 1, size_t height = 1);

            void bind();
            void unbind();

            void resize(size_t width, size_t height);

        private:
            renderbuffer_handler m_handler;
            GLenum m_format;
            uint32_t m_samples_count;
        };
    } // namespace detail


    class render_pass
    {
        friend class renderer;

    public:
        render_pass(const pass_descriptor&, memory::pool_view<texture>& textures);
        void resize(size_t width, size_t height, memory::pool_view<texture>& textures);

        void begin();
        void end();

    private:
        void clear_pass();

        detail::framebuffer_handler m_framebuffer_handler;
        std::optional<detail::framebuffer_handler> m_msaa_resolve_framebuffer;
        ::renderer::pass_state m_state;
        size_t m_width, m_height;
        std::vector<::renderer::attachment_descriptor> m_attachments_list;
        std::vector<detail::renderbuffer> m_renderbuffers;
    };
} // namespace renderer::gl
