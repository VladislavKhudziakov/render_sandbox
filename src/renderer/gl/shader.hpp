

#pragma once

#include <renderer/gl/raii_storage.hpp>

#include <cinttypes>
#include <glad/glad.h>

namespace renderer
{
    struct shader_descriptor;
    struct shader_stage;
} // namespace renderer

namespace renderer::gl
{
    namespace detail
    {
        struct shader_create_policy
        {
            static void create(uint32_t* handler)
            {
                *handler = glCreateProgram();
            }
        };

        struct shader_destroy_policy
        {
            static void destroy(uint32_t* handler)
            {
                return glDeleteProgram(*handler);
            }
        };

        struct shader_raii_wrap
        {
            explicit shader_raii_wrap(GLuint handler)
                : m_handler(handler)
            {
            }

            shader_raii_wrap(const shader_raii_wrap&) = delete;
            shader_raii_wrap& operator=(const shader_raii_wrap&) = delete;

            shader_raii_wrap(shader_raii_wrap&& src) noexcept
            {
                *this = std::move(src);
            }

            shader_raii_wrap& operator=(shader_raii_wrap&& src) noexcept
            {
                if (this != &src) {
                    m_handler = src.m_handler;
                    src.m_handler = 0;
                }

                return *this;
            }


            ~shader_raii_wrap()
            {
                glDeleteShader(m_handler);
            }

            operator GLuint()
            {
                return m_handler;
            }

        private:
            GLuint m_handler;
        };
    } // namespace detail


    class shader
    {
    public:
        shader(const ::renderer::shader_descriptor&);
        void bind();
        void unbind();

    private:
        detail::shader_raii_wrap compile_shader(const ::renderer::shader_stage&);
        raii_storage<detail::shader_create_policy, detail::shader_destroy_policy> m_handler;
    };
} // namespace renderer::gl
