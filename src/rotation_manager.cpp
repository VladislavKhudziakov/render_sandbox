

#include "rotation_manager.hpp"

#include <cube.hpp>

rubiks_cube::rotation_manager::rotation_manager(size_t size)
: m_size(size)
{
    m_rows[x_axis].resize(size);
    m_rows[y_axis].resize(size);
    m_rows[z_axis].resize(size);
}


void rubiks_cube::rotation_manager::update()
{
    assert(m_rotation_animations.size() == 1 || m_rotation_animations.size() == 0);

    for (auto& anim : m_rotation_animations) {
        auto offset = 0.05 * anim.direction;
        if (anim.row->angle + offset >= anim.bind_point) {
            anim.row->need_cubes_update_position = true;
            anim.row->update_direction = round(sinf(anim.bind_point));
            anim.row->angle_offset = anim.bind_point - anim.row->angle;
            m_rotation_animations.pop_back();
        } else {
            anim.row->angle_offset = offset;
            anim.row->angle += offset;
        }
    }
}


void rubiks_cube::rotation_manager::rotate(uint32_t index, float angle_offset)
{
    uint32_t rot_axis;

    if (m_axis_acquired.test(x_axis)) {
        rot_axis = x_axis;
    } else if (m_axis_acquired.test(y_axis)) {
        rot_axis = y_axis;
    } else if (m_axis_acquired.test(z_axis)) {
        rot_axis = z_axis;
    } else {
        return;
    }


    m_row_index = index;
    auto& row = m_rows[rot_axis][index];
    row.need_cubes_update_position = false;

    row.angle_offset = angle_offset;

    if (angle_offset > 0) {
        if (row.angle + angle_offset > M_PI_2) {
            row.angle_offset = M_PI_2 - row.angle;
            row.angle = 0;
            row.need_cubes_update_position = true;
        } else {
            row.angle += angle_offset;
            row.angle_offset = angle_offset;
        }
        row.update_direction = -1;
    } else if (angle_offset < 0) {
        if (row.angle - angle_offset < -M_PI_2) {
            row.angle_offset = row.angle + M_PI_2;
            row.angle = 0;
            row.need_cubes_update_position = true;
        } else {
            row.angle += angle_offset;
            row.angle_offset = angle_offset;
        }
        row.update_direction = 1;
    }
}


void rubiks_cube::rotation_manager::try_acquire_rotation_axis(uint32_t axis)
{
    if (!m_rotation_animations.empty()) {
        return;
    }

    if (!m_axis_acquired.any() && !m_axis_acquired.test(axis)) {
        m_axis_acquired.set(axis);
    }
}


void rubiks_cube::rotation_manager::release_rotation_axis()
{
    uint32_t rot_axis;

    if (m_axis_acquired.test(x_axis)) {
        rot_axis = x_axis;
    } else if (m_axis_acquired.test(y_axis)) {
        rot_axis = y_axis;
    } else if (m_axis_acquired.test(z_axis)) {
        rot_axis = z_axis;
    } else {
        return;
    }

    m_axis_acquired.reset();

    auto& row = m_rows[rot_axis][m_row_index];

    row.angle_offset = 0;
    m_row_index = -1;

    auto& anim = m_rotation_animations.emplace_back();

    anim.row = &row;

//    if (row.angle > 0) {
//        if (row.angle < M_PI_4) {
//            anim.direction = -1;
//            anim.bind_point = 0;
//        } else {
//            anim.direction = 1;
//            anim.bind_point = M_PI_2;
//        }
//    } else {
//        if (row.angle > -M_PI_4) {
//            anim.direction = 1;
//            anim.bind_point = 0;
//        } else {
//            anim.direction = -1;
//            anim.bind_point = -M_PI_2;
//        }
//    }
}


void rubiks_cube::rotation_manager::rotate_cube(rubiks_cube::cube& c)
{
    auto pos = c.get_position();

    pos.x = pos.x + (m_size / 2);
    pos.y = pos.y + (m_size / 2);
    pos.z = -(pos.z - (m_size / 2));

    auto& row_x = m_rows[x_axis][pos.x];
    auto& row_y = m_rows[y_axis][pos.y];
    auto& row_z = m_rows[z_axis][pos.z];

    c.rotate(x_axis, row_x.angle_offset);
    c.rotate(y_axis, row_y.angle_offset);
    c.rotate(z_axis, row_z.angle_offset);

    if (row_x.need_cubes_update_position) {
        c.update_position(x_axis, row_x.update_direction);
    } else if (row_y.need_cubes_update_position) {
        c.update_position(y_axis, row_y.update_direction);
    } else if (row_z.need_cubes_update_position) {
        c.update_position(z_axis, row_z.update_direction);
    }
}
