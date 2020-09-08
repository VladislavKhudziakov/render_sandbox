

#include "rubiks_cube.hpp"


rubiks_cube::rubiks_cube::rubiks_cube(renderer::renderer* renderer, size_t size, float cube_side)
    : m_size(size)
{
    float rubiks_cube_size = size * cube_side;
    float offset = cube_side * 0.5f;
    float x_pos = -rubiks_cube_size / 2 + offset;
    float y_pos = -rubiks_cube_size / 2 + offset;
    float z_pos = -rubiks_cube_size / 2 + offset;
    float stride = cube_side;

    m_cubes.reserve(size * size * size);

    m_x_rows.resize(size);
    m_y_rows.resize(size);
    m_z_rows.resize(size);
    m_z_rows[0].rotate(M_PI_4 - 0.1);


    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            for (int z = 0; z < size; ++z) {

                auto& new_cube = m_cubes.emplace_back(
                    renderer,
                    get_cube_faces_color({x, y, z}));
                new_cube.side = cube_side;
                new_cube.translation = {x_pos, y_pos, z_pos};
                new_cube.position = {x, y, z};
                z_pos += stride;
            }
            z_pos = -rubiks_cube_size / 2 + offset;
            y_pos += stride;
        }
        y_pos = -rubiks_cube_size / 2 + offset;
        x_pos += stride;
    }
}


void rubiks_cube::rubiks_cube::draw()
{
    auto rot_m = math::rotation_x(rotation.x) * math::rotation_y(rotation.y) * math::rotation_z(rotation.z);
    auto t_m = math::translation(translation);
    auto s_m = math::scale(scale);

    auto self_transform = rot_m * t_m * s_m;

    for (auto& cube : m_cubes) {
        auto rows_transform =
            m_x_rows[cube.position.x].get_transformation() *
            m_y_rows[cube.position.y].get_transformation() *
            m_z_rows[cube.position.z].get_transformation();

        cube.parent_transform = rows_transform * self_transform * parent_transform;
        cube.draw();
    }
}
rubiks_cube::color_data rubiks_cube::rubiks_cube::get_cube_faces_color(math::ivec3 pos)
{
    ::rubiks_cube::color_data res{
        .pos_x = {1, 0, 0},
        .neg_x = {0, 1, 0},
        .pos_y = {0, 0, 1},
        .neg_y = {1, 1, 0},
        .pos_z = {1, 0.5, 0},
        .neg_z = {1, 1, 1}};

    if (pos.x == 0) {
        res.pos_x[0] = 0;
    } else if (pos.x == m_size - 1) {
        res.neg_x[1] = 0;
    } else {
        res.pos_x[0] = 0;
        res.neg_x[1] = 0;
    }

    if (pos.y == 0) {
        res.pos_y[2] = 0;
    } else if (pos.y == m_size - 1) {
        res.neg_y[0] = 0;
    } else {
        res.pos_y[2] = 0;
        res.neg_y[0] = 0;
        res.neg_y[1] = 0;
    }

    if (pos.z == 0) {
        res.pos_z[1] = 0;
        res.pos_z[2] = 0;
    } else if (pos.z == m_size - 1) {
        res.neg_z[0] = 0;
        res.neg_z[1] = 0;
        res.neg_z[2] = 0;
    } else {
        res.pos_z[0] = 0;
        res.pos_z[1] = 0;

        res.neg_z[0] = 0;
        res.neg_z[1] = 0;
        res.neg_z[2] = 0;
    }

    return res;
}


void rubiks_cube::rubiks_cube::update()
{
    for (auto& row : m_x_rows) {
        row.update(math::rotation_x);
    }

    for (auto& row : m_y_rows) {
        row.update(math::rotation_y);
    }

    for (auto& row : m_z_rows) {
        row.update(math::rotation_z);
    }
}


void rubiks_cube::rubiks_cube::rotate_z_row(size_t index, float angle)
{
}


void rubiks_cube::rubiks_cube::rotate_y_row(size_t index, float angle)
{
}


void rubiks_cube::rubiks_cube::rotate_x_row(size_t index, float angle)
{
}
