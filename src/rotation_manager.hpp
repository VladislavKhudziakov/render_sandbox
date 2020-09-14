


#pragma once

#include <math/matrix.hpp>

#include <array>
#include <vector>
#include <functional>
#include <queue>


namespace rubiks_cube
{
    class cube;

    struct row
    {
        float angle{0};
        float angle_offset;
        int update_direction = 0;
        bool need_cubes_update_position = false;
    };

    struct rotation_animation
    {
        int32_t direction;
        float bind_point;
        row* row;
    };

    class rotation_manager
    {
    public:
        using cube_position_rotater = std::pair<std::function<math::ivec3(math::ivec3)>, std::function<math::ivec3(math::ivec3)>>;

        explicit rotation_manager(size_t size);

        enum axis
        {
            x,
            y,
            z
        };

        void acquire_row(axis axis, uint32_t index);
        void release_row();
        void rotate(float angle);
        bool is_any_row_acquired();
        void rotate_cube(cube& c);
        void update();

    private:
        std::array<std::vector<row>, 3> m_rows;
        std::queue<rotation_animation> m_animations_queue;
        size_t m_size;
        uint32_t m_acquired_row{uint32_t(-1)};
        axis m_acquired_axis{axis::x};
    };
} // namespace rubiks_cube
