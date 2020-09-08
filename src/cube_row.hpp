


#pragma once

#include <cube.hpp>

namespace rubicks_cube
{
    class cube_row
    {
    public:
        explicit cube_row(renderer::renderer* renderer, size_t cubes_count = 3);

        void draw();
        void rotate(float angle);

        math::mat4 parent_transform;

    private:
        float angle = 0;
        std::vector<cube> m_cubes;
    };
}

