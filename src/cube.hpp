


#pragma once

#include <renderer/renderer.hpp>
#include <math/matrix.hpp>

namespace rubiks_cube
{
    struct face
    {
        math::vec4 normal;
        math::vec4 color;
    };

    class cube
    {
    public:
        enum rotation_angle
        {
            x_axis,
            y_axis,
            z_axis
        };

        std::array<face, 6> faces{
            face{
                .normal = {1, 0, 0, 0},
            },
            {.normal = {-1, 0, 0, 0}},
            {.normal = {0, 1, 0, 0}},
            {.normal = {0, -1, 0, 0}},
            {.normal = {0, 0, 1, 0}},
            {.normal = {0, 0, -1, 0}}};

        math::vec4 color{0, 0, 0, 0};

        explicit cube(math::ivec3 translation);
        void rotate(uint32_t axis, float angle);
        void update_position(uint32_t axis, int direction);
        math::ivec3 get_position();
        math::mat4 get_transformation();

    private:
        math::vec3 m_translation;
        math::ivec3 m_position;
        std::array<float, 3> m_rotation{0, 0, 0};
        std::array<float, 3> m_bind_angle{0, 0, 0};

        inline static std::function<math::mat4(float)> orig_x_rotator = math::rotation_x;
        inline static std::function<math::mat4(float)> orig_y_rotator = math::rotation_y;
        inline static std::function<math::mat4(float)> orig_z_rotator = math::rotation_z;

        std::function<math::mat4(float)> m_x_rotator = orig_x_rotator;
        std::function<math::mat4(float)> m_y_rotator = orig_y_rotator;
        std::function<math::mat4(float)> m_z_rotator = orig_z_rotator;

        float m_x_rot_dir = 1;
        float m_y_rot_dir = 1;
        float m_z_rot_dir = 1;
    };
} // namespace rubiks_cube
