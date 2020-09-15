


#pragma once

#include <renderer/renderer.hpp>
#include <math/matrix.hpp>

namespace rubiks_cube
{
    struct face
    {
        math::vec4 normal;
        math::ubvec4 color;
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
        math::ivec3 get_position() const;
        math::mat4 get_transformation();

    private:
        math::ivec3 m_position;
        std::array<float, 3> m_rotation{0, 0, 0};
    };
} // namespace rubiks_cube
