

#pragma once

#include <renderer/gl/raii_storage.hpp>
#include <renderer/gl/bind_guard.hpp>

#include <glad/glad.h>

namespace renderer::gl
{
    namespace detail
    {
        struct buffer_create_policy
        {
            static void create(uint32_t* handler)
            {
                return glGenBuffers(1, handler);
            }
        };

        struct buffer_destroy_policy
        {
            static void destroy(uint32_t* handler)
            {
                return glDeleteBuffers(1, handler);
            }
        };
    } // namespace detail

    template<GLenum BufferType>
    class buffer
    {
    public:
        void bind()
        {
            glBindBuffer(BufferType, m_handler);
        }

        void unbind()
        {
            glBindBuffer(BufferType, 0);
        }

        void load_data(const void* data, size_t data_size, bool static_usage = true)
        {
            glBufferData(BufferType, data_size, data, static_usage ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        }

    private:
        raii_storage<detail::buffer_create_policy, detail::buffer_destroy_policy> m_handler;
    };

    using vertex_buffer = buffer<GL_ARRAY_BUFFER>;
    using element_buffer = buffer<GL_ELEMENT_ARRAY_BUFFER>;
    using uniform_buffer = buffer<GL_UNIFORM_BUFFER>;
} // namespace renderer::gl
