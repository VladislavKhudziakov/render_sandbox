


#pragma once

#include <renderer/renderer.hpp>
#include <math/matrix.hpp>

namespace rubiks_cube
{
    struct color_data
    {
        float pos_x[3];
        float neg_x[3];
        float pos_y[3];
        float neg_y[3];
        float pos_z[3];
        float neg_z[3];
    };

    class cube
    {
    public:
        constexpr static auto default_side = 2.0f;

        explicit cube(renderer::renderer* renderer);
        explicit cube(renderer::renderer* renderer, const color_data&);
        cube(const cube&) = delete;
        cube& operator=(const cube&) = delete;
        cube(cube&&) noexcept;
        cube& operator=(cube&&) noexcept;
        ~cube();

        void draw();

        float side = 2.0f;

        math::ivec3 position{0, 0, 0};
        math::vec3 translation{0, 0, 0};

        math::mat4 parent_transform;
    private:
        renderer::renderer* m_renderer;

        renderer::mesh_handler m_mesh;
        renderer::shader_handler m_shader;
        renderer::parameters_list_handler m_params_list;
    };
}

