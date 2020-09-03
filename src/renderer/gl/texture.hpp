

#pragma once

#include <renderer/gl/raii_storage.hpp>

#include <glad/glad.h>

#include <cinttypes>

namespace renderer
{
    struct texture_descriptor;
}

namespace renderer::gl
{
    namespace detail
    {
        struct texture_create_policy
        {
            static void create(uint32_t* handler)
            {
                return glGenTextures(1, handler);
            }
        };

        struct texture_destroy_policy
        {
            static void destroy(uint32_t* handler)
            {
                return glDeleteTextures(1, handler);
            }
        };

        using texture_handler = raii_storage<texture_create_policy, texture_destroy_policy>;
    } // namespace detail

    class texture
    {
        friend class renderer;

    public:
        explicit texture(const texture_descriptor&);
        void bind();
        void unbind();

    private:
        void load_2d_data(const texture_descriptor&);
        void load_cube_data(const texture_descriptor&);
        void load_3d_data(const texture_descriptor&);

        detail::texture_handler m_handler;
        GLenum m_type;
    };
} // namespace renderer::gl
