

#include "rubicks_cube.hpp"


rubicks_cube::rubicks_cube::rubicks_cube(renderer::renderer* renderer, size_t size)
{
    int min_offset = -int(size - 1) / 2;
    int max_offset = int(size) / 2;

    m_cubes.reserve(size * size * size);

    m_x_rows.resize(size);
    m_y_rows.resize(size);
    m_z_rows.resize(size);

    for (int x = min_offset, xpos = 0; x <= max_offset; ++x, ++xpos) {
        for (int y = min_offset, ypos = 0; y <= max_offset; ++y, ++ypos) {
            for (int z = min_offset, zpos = 0; z <= max_offset; ++z, ++zpos) {
                auto& new_cube = m_cubes.emplace_back(renderer);
                new_cube.translation = {new_cube.side * x, new_cube.side * y, new_cube.side * z};
                new_cube.position = {xpos, ypos, zpos};
            }
        }
    }
}


void rubicks_cube::rubicks_cube::draw()
{
    auto rot_m = math::rotation_x(rotation.x) *  math::rotation_y(rotation.y) *  math::rotation_z(rotation.z);
    auto t_m = math::translation(translation);
    auto s_m = math::scale(scale);

    for (auto& x_row : m_x_rows) {
        x_row.matrix = math::rotation_x(x_row.angle);
    }

    for (auto& y_row : m_y_rows) {
        y_row.matrix = math::rotation_y(y_row.angle);
    }

    for (auto& z_row : m_z_rows) {
        z_row.matrix = math::rotation_z(z_row.angle);
    }

    auto self_transform = rot_m * t_m * s_m;

    for (auto& cube : m_cubes) {
        auto rows_transform = m_x_rows[cube.position.x].matrix * m_y_rows[cube.position.y].matrix * m_z_rows[cube.position.z].matrix;
        cube.parent_transform = rows_transform * self_transform * parent_transform;
        cube.draw();
    }
}
