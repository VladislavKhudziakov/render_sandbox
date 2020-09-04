

#pragma once

#include <renderer/gl/raii_storage.hpp>
#include <renderer/gl/texture.hpp>
#include <renderer/renderer.hpp>

#include <cinttypes>
#include <glad/glad.h>

namespace renderer
{
    class pass_descriptor;
}

namespace renderer::gl
{
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
    } // namespace detail


    class render_pass
    {
        friend class renderer;

    public:
        render_pass(const pass_descriptor&, std::vector<gl::texture>&);
        void resize(size_t width, size_t height, std::vector<gl::texture>&);

        void begin();
        void end();

    private:
        void clear_pass();

        detail::framebuffer_handler m_framebuffer_handler;
        ::renderer::pass_state m_state;
        size_t m_width, m_height;
        std::vector<::renderer::attachment_descriptor> m_attachments_list;
    };
} // namespace renderer::gl
