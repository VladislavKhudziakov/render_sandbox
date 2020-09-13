


#pragma once

#include <cube.hpp>
#include <rotation_manager.hpp>
#include <math/ray.hpp>

namespace rubiks_cube
{
    class rubiks_cube
    {
    public:
        explicit rubiks_cube(renderer::renderer* renderer, size_t size = 5);

        void update();

        void draw();

        bool hit(math::ray ray, size_t& i);

        math::mat4 parent_transform;

        math::vec3 translation{0, 0, 0};
        math::vec3 scale{1, 1, 1};
        math::vec3 rotation{0, 0, 0};

        rotation_manager rotation_manager;

    private:
        std::vector<cube> m_cubes;

        renderer::shader_handler m_draw_shader;
        renderer::parameters_list_handler m_params_list;
        renderer::mesh_handler m_cube_mesh;

        renderer::renderer* m_renderer;

        math::mat4 m_transform;

        size_t m_size;
    };
} // namespace rubiks_cube
