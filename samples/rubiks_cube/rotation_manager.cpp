

#include "rotation_manager.hpp"

#include "cube.hpp"

rubiks_cube::rotation_manager::rotation_manager(size_t size)
    : m_size(size)
{
    m_rows[axis::x].resize(size);
    m_rows[axis::y].resize(size);
    m_rows[axis::z].resize(size);
}


void rubiks_cube::rotation_manager::update(double delta_time)
{
    if (m_animations_queue.empty()) {
        while (!m_disable_update_rows_queue.empty()) {
            auto* row = m_disable_update_rows_queue.back();
            row->need_cubes_update_position = false;
            m_disable_update_rows_queue.pop();
        }
        return;
    }

    auto& anim = m_animations_queue.front();
    auto offset = delta_time * 10.0f * anim.direction;

    auto animation_finished = (anim.direction < 0 ? (anim.row->angle + offset <= anim.bind_point) : (anim.row->angle + offset >= anim.bind_point));

    if (animation_finished) {
        anim.row->need_cubes_update_position = true;
        anim.row->update_direction = anim.direction;

        m_animations_queue.pop();

        anim.row->angle_offset = 0;
        anim.row->update_direction = int(round(sin(anim.bind_point)));
        m_rows[m_acquired_axis][m_acquired_row].angle = 0;
        m_acquired_row = uint32_t(-1);
        m_disable_update_rows_queue.push(anim.row);
    } else {
        anim.row->angle_offset = offset;
        anim.row->angle += offset;
    }
}


void rubiks_cube::rotation_manager::rotate(float angle_offset)
{
    if (!is_any_row_acquired()) {
        return;
    }

    auto& row = m_rows[m_acquired_axis][m_acquired_row];

    row.need_cubes_update_position = false;

    row.angle_offset = angle_offset;

    if (angle_offset > 0) {
        if (row.angle + angle_offset > M_PI_2) {
            row.angle_offset = M_PI_2 - row.angle;
            row.angle = 0;
            row.need_cubes_update_position = true;
            m_disable_update_rows_queue.emplace(&row);
        } else {
            row.angle += angle_offset;
            row.angle_offset = angle_offset;
        }
        row.update_direction = 1;
    } else if (angle_offset < 0) {
        if (row.angle - angle_offset < -M_PI_2) {
            row.angle_offset = row.angle + M_PI_2;
            row.angle = 0;
            row.need_cubes_update_position = true;
            m_disable_update_rows_queue.emplace(&row);
        } else {
            row.angle += angle_offset;
            row.angle_offset = angle_offset;
        }
        row.update_direction = -1;
    }
}


void rubiks_cube::rotation_manager::rotate_cube(rubiks_cube::cube& c)
{
    auto pos = c.get_position();

    pos.x = pos.x + (m_size / 2);
    pos.y = pos.y + (m_size / 2);
    pos.z = -(pos.z - (m_size / 2));

    auto& row_x = m_rows[axis::x][pos.x];
    auto& row_y = m_rows[axis::y][pos.y];
    auto& row_z = m_rows[axis::z][pos.z];

    c.rotate(axis::x, row_x.angle_offset);
    c.rotate(axis::y, row_y.angle_offset);
    c.rotate(axis::z, row_z.angle_offset);

    if (row_x.need_cubes_update_position) {
        c.update_position(axis::x, row_x.update_direction);
    } else if (row_y.need_cubes_update_position) {
        c.update_position(axis::y, row_y.update_direction);
    } else if (row_z.need_cubes_update_position) {
        c.update_position(axis::z, row_z.update_direction);
    }
}


void rubiks_cube::rotation_manager::acquire_row(rubiks_cube::rotation_manager::axis axis, uint32_t index)
{
    if (index < 0 || index >= m_size) {
        return;
    }

    m_acquired_row = index;
    m_acquired_axis = axis;
}


void rubiks_cube::rotation_manager::release_row()
{
    auto& row = m_rows[m_acquired_axis][m_acquired_row];
    auto& anim = m_animations_queue.emplace();

    if (row.angle >= 0 && row.angle < M_PI_4) {
        anim.direction = -1;
        anim.bind_point = 0;
    } else if (row.angle >= M_PI_4) {
        anim.direction = 1;
        anim.bind_point = M_PI_2;
    } else if (row.angle < 0 && row.angle > -M_PI_4) {
        anim.direction = 1;
        anim.bind_point = 0;
    } else {
        anim.direction = -1;
        anim.bind_point = -M_PI_2;
    }

    anim.row = &row;
}


bool rubiks_cube::rotation_manager::is_any_row_acquired()
{
    return m_acquired_row != uint32_t(-1);
}
