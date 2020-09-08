


#pragma once

#include <cube.hpp>

namespace rubiks_cube
{
    class rubiks_cube
    {
    public:
        explicit rubiks_cube(renderer::renderer* renderer, size_t size = 4, float cube_side = 2);

        void update();

        void draw();

        math::mat4 parent_transform;

        math::vec3 translation {0,0, 0};
        math::vec3 scale {1, 1, 1};
        math::vec3 rotation {0, 0, 0};

        void rotate_z_row(size_t index, float angle);
        void rotate_y_row(size_t index, float angle);
        void rotate_x_row(size_t index, float angle);

    private:
        color_data get_cube_faces_color(math::ivec3);

        class row
        {
        public:
            template<typename Functional>
            void update(Functional f)
            {
                if (m_rotated) {
                    m_transformation = f(m_angle);
                    m_rotated = false;
                    return;
                }

                if (m_angle == m_angle_bind_point) {
                    return;
                }

                if (m_angle < m_angle_bind_point - M_PI_4) {
                    m_angle_bind_point = m_angle_bind_point - M_PI_2;
                }

                if (m_angle > m_angle_bind_point + M_PI_4) {
                    m_angle_bind_point = m_angle_bind_point + M_PI_2;
                }

                if (m_angle < m_angle_bind_point) {
                    m_angle = m_angle + 0.01 > m_angle_bind_point ? m_angle_bind_point : m_angle + 0.01f;
                } else {
                    m_angle = m_angle - 0.01 < m_angle_bind_point ? m_angle_bind_point : m_angle - 0.01f;
                }

                m_transformation = f(m_angle);
            }

            const math::mat4 get_transformation()
            {
                return m_transformation;
            }

            void rotate(float angle)
            {
                m_angle += angle;

                if (m_angle < m_angle_bind_point - M_PI_4) {
                    m_angle_bind_point = m_angle_bind_point - M_PI_2;
                }

                if (m_angle > m_angle_bind_point + M_PI_4) {
                    m_angle_bind_point = m_angle_bind_point + M_PI_2;
                }

                m_rotated = true;
            }

        private:
            float m_angle;
            float m_angle_bind_point = 0;
            bool m_rotated = false;
            math::mat4 m_transformation;
        };

        struct row_transform
        {
            float angle {0};
            math::mat4 matrix;
        };

        std::vector<cube> m_cubes;

        std::vector<row> m_x_rows;
        std::vector<row> m_y_rows;
        std::vector<row> m_z_rows;

//        std::vector<row_transform> m_x_rows;
//        std::vector<row_transform> m_y_rows;
//        std::vector<row_transform> m_z_rows;

        size_t m_size;
    };
}

