

#include "cube_row.hpp"
rubicks_cube::cube_row::cube_row(renderer::renderer* renderer, size_t cubes_count)
{
    m_cubes.reserve(cubes_count);

//    auto xx = -(int(cubes_count - 1) / 2);
//    auto xxx = cubes_count / 2;
//
//    for (int x = -1; x <= 1; ++x) {
//        for (int z = -1; z <= 1; ++z) {
//            auto& new_cube = m_cubes.emplace_back(renderer);
//            new_cube.position.x = x * new_cube.side;
//            new_cube.position.z = z * new_cube.side;
//        }
//    }
}

void rubicks_cube::cube_row::draw()
{
    for (auto& cube : m_cubes) {
        cube.parent_transform = parent_transform;
        cube.draw();
    }
}

void rubicks_cube::cube_row::rotate(float angle)
{
}
