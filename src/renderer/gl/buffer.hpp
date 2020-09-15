

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

    template<GLenum BufferType, bool static_usage = true>
    class buffer
    {
        friend class renderer;

    public:
        explicit buffer(size_t size)
            : m_storage_size(size)
        {
            if (!static_usage) {
                bind_guard bind(*this);
                glBufferData(BufferType, size, nullptr, GL_DYNAMIC_DRAW);
            }
        }


        void bind()
        {
            glBindBuffer(BufferType, m_handler);
        }

        void unbind()
        {
            glBindBuffer(BufferType, 0);
        }

        void load_data(const void* data)
        {
            if constexpr (!static_usage) {
                glBufferSubData(BufferType, 0, (GLsizeiptr) m_storage_size, data);
            } else {
                glBufferData(BufferType, m_storage_size, data, GL_STATIC_DRAW);
            }
        }

    private:
        raii_storage<detail::buffer_create_policy, detail::buffer_destroy_policy> m_handler;
        size_t m_storage_size;
    };

    using vertex_buffer = buffer<GL_ARRAY_BUFFER>;
    using element_buffer = buffer<GL_ELEMENT_ARRAY_BUFFER>;
    using uniform_buffer = buffer<GL_UNIFORM_BUFFER, false>;
} // namespace renderer::gl
