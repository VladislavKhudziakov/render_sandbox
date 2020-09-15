


#pragma once

#include <cube.hpp>
#include <rotation_manager.hpp>
#include <math/ray.hpp>

namespace rubiks_cube
{
    class rubiks_cube
    {
    public:
        enum face
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
        rubiks_cube(const rubiks_cube&) = delete;
        rubiks_cube(rubiks_cube&&) = delete;
        rubiks_cube& operator=(const rubiks_cube&) = delete;
        rubiks_cube& operator=(rubiks_cube&&) = delete;
        ~rubiks_cube();

        void update();
        void draw();
        bool hit(math::ray ray, face& face, math::vec3& hit_point);
        math::ivec2 get_row_col_by_hit_pos(face& face, math::vec3 hit_point);
        math::ivec2 get_face_rotation_axis(face face);

        void acquire();
        void release();
        void rotate(math::vec3);
        bool is_acquired() const;

        math::mat4 parent_transform;

        rotation_manager rotation_manager;

    private:
        void get_faces_color_by_position(cube&, math::ivec3 pos);
        void upload_cube_faces_texture_data();
        void create_cubes_colors_texture();

        std::vector<math::ubvec4> m_faces_texture_data;

        std::vector<cube> m_cubes;

        renderer::shader_handler m_draw_shader;
        renderer::parameters_list_handler m_params_list;
        renderer::mesh_handler m_cube_mesh;
        renderer::texture_handler m_cubes_faces_texture;

        renderer::renderer* m_renderer;

        math::mat4 m_transform;
        math::vec3 m_rotation{0, 0, 0};

        size_t m_size;

        bool m_is_acquired = false;
    };
} // namespace rubiks_cube
