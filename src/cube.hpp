


#pragma once

#include <renderer/renderer.hpp>
#include <math/matrix.hpp>

namespace rubiks_cube
{
    class cube
    {
    public:
        enum rotation_angle
        {
            x_axis, y_axis, z_axis
        };

        math::vec4 color;

        explicit cube(math::ivec3 translation);
        void rotate(uint32_t axis, float angle);
        void update_position(uint32_t axis, int direction);
        math::ivec3 get_position();
        math::mat4 get_transformation();

    private:
        math::mat4 m_transformation;
        math::ivec3 m_position;
        std::array<float, 3> m_rotation {0, 0, 0};
    };
}

