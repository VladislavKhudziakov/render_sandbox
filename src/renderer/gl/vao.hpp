


#pragma once

#include <renderer/gl/raii_storage.hpp>
#include <glad/glad.h>

namespace renderer
{
    class mesh_layout_descriptor;
}


namespace renderer::gl
{
    namespace detail
    {
        struct vao_create_policy
        {
            static void create(uint32_t* handler)
            {
                return glGenVertexArrays(1, handler);
            }
        };

        struct vao_destroy_policy
        {
            static void destroy(uint32_t* handler)
            {
                return glDeleteVertexArrays(1, handler);
            }
        };
    } // namespace detail


    class vao
    {
    public:
        explicit vao(const mesh_layout_descriptor&);
        void draw();
        void draw_instanced(uint32_t instances_count);
        void bind();
        void unbind();

    private:
        raii_storage<detail::vao_create_policy, detail::vao_destroy_policy> m_handler;
        size_t m_indices_count{0};
        size_t m_vertices_count{0};
        GLenum m_indices_format{0};
        GLenum m_geometry_topology{0};
    };
} // namespace renderer::gl
