


#pragma once

#include <cube.hpp>
#include <rotation_manager.hpp>
#include <math/ray.hpp>

namespace rubiks_cube
{
    class rubiks_cube
    {
    public:
        enum faces
        {
            pos_x,
            neg_x,
            pos_y,
            neg_y,
            pos_z,
            neg_z,
            size
        };

        explicit rubiks_cube(renderer::renderer* renderer, size_t size = 5);

        void update();

        void draw();

        bool hit(math::ray ray, faces& face, math::vec3& hit_point);
        math::ivec2 get_row_col_by_hit_pos(faces& face, math::vec3 hit_point);

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
