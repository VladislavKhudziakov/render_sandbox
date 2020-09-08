


#pragma once

#include <cube.hpp>

namespace rubicks_cube
{
    class rubicks_cube
    {
    public:
        explicit rubicks_cube(renderer::renderer* renderer, size_t size = 4);

        void draw();

        math::mat4 parent_transform;

        math::vec3 translation {0,0, 0};
        math::vec3 scale {1, 1, 1};
        math::vec3 rotation {0, 0, 0};

    private:
        struct row_transform
        {
            float angle {0};
            math::mat4 matrix;
        };

        std::vector<cube> m_cubes;

        std::vector<row_transform> m_x_rows;
        std::vector<row_transform> m_y_rows;
        std::vector<row_transform> m_z_rows;
    };
}

