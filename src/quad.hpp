


#pragma once

#include <math/matrix.hpp>
#include <renderer/renderer.hpp>

namespace rubiks_cube
{
    class indication_quad
    {
    public:
        explicit indication_quad(renderer::renderer*);
        ~indication_quad();
        indication_quad(const indication_quad&) = delete;
        indication_quad(indication_quad&&) = delete;
        indication_quad& operator=(const indication_quad&) = delete;
        indication_quad& operator=(indication_quad&&) = delete;

        math::vec3 position;
        math::vec3 scale;

        void update();
        void draw();

    private:
        renderer::shader_handler m_shader;
        renderer::mesh_handler m_mesh;
        renderer::parameters_list_handler m_params_list;
        renderer::texture_handler m_draw_texture;

        renderer::renderer* m_renderer;
    };
} // namespace rubiks_cube
