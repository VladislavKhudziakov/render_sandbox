


#pragma once

#include <math/matrix.hpp>

#include <array>
#include <vector>
#include <functional>


namespace rubiks_cube
{
    class cube;

    struct row
    {
        float angle{0};
        float angle_offset;
        bool need_cubes_update_position = false;
    };

    struct animation
    {
        int32_t direction;
        float bind_angle;
        row* row;
    };

    class rotation_manager
    {
    public:
        using cube_position_rotater = std::pair<std::function<math::ivec3(math::ivec3)>, std::function<math::ivec3(math::ivec3)>>;

        explicit rotation_manager(size_t size);

        enum
        {
            x_axis,
            y_axis,
            z_axis
        };

        void try_acquire_rotation_axis(uint32_t axis);
        void release_rotation_axis();
        void rotate(uint32_t index, float angle);

        void update();

    private:
        std::array<std::vector<row>, 3> m_rows;
        std::bitset<3> m_axis_acquired;
        size_t m_size;
        uint32_t m_row_index;
    };
}
