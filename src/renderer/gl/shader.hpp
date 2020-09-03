

#pragma once

#include <renderer/gl/raii_storage.hpp>

#include <cinttypes>
#include <string>
#include <unordered_map>

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

        struct stage_create_policy
        {
            static void create(uint32_t* handler, GLenum stage_name)
            {
                *handler = glCreateShader(stage_name);
            }
        };

        struct stage_destroy_policy
        {
            static void destroy(uint32_t* handler)
            {
                return glDeleteShader(*handler);
            }
        };

        using stage_handler = raii_storage<stage_create_policy, stage_destroy_policy>;
        using shader_handler = raii_storage<shader_create_policy, shader_destroy_policy>;
    } // namespace detail


    class shader
    {
        friend class renderer;

    public:
        explicit shader(const ::renderer::shader_descriptor&);
        void bind();
        void unbind();

    private:
        detail::stage_handler compile_shader(const ::renderer::shader_stage&);
        detail::shader_handler m_handler;
        std::unordered_map<std::string, uint32_t> m_samplers;
    };
} // namespace renderer::gl
