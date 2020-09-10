

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
    if (m_axis_acquired.any()) {
//        for (int i = 0; i < m_rows.size(); ++i) {
//            auto& rows_list = m_rows[i];
//            for (auto& row : rows_list) {
////                for (auto& cube : row.cubes) {
////                    cube->rotate(i, row.angle);
////                }
//            }
//        }
//        return;
    }

//    for (int i = 0; i < m_rows.size(); ++i) {
//        auto& row_list = m_rows[i];
//        for (auto& row : row_list) {
//            if (row.angle < row.bind_point - M_PI_4) {
//                row.bind_point = row.bind_point - M_PI_2;
//            }
//
//            if (row.angle > row.bind_point + M_PI_4) {
//                row.bind_point = row.bind_point + M_PI_2;
//            }
//
//            if (row.angle < row.bind_point) {
//                row.angle = row.angle + 0.05 > row.bind_point ? row.bind_point : row.angle + 0.05f;
//            } else {
//                row.angle = row.angle - 0.05 < row.bind_point ? row.bind_point : row.angle - 0.05f;
//            }
//
//            for (auto& cube : row.cubes) {
//                cube->rotate(i, row.angle);
//            }
//        }
//    }
}


void rubiks_cube::rotation_manager::rotate(uint32_t index, float angle)
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
    m_rows[rot_axis][index].angle = angle;
    m_rows[rot_axis][index].angle_offset = angle;

//    if (m_rows[rot_axis][index].angle > M_PI) {
//        m_rows[rot_axis][index].angle -= M_PI;
//    }
//
//    if (m_rows[rot_axis][index].angle < -M_PI) {
//        m_rows[rot_axis][index].angle += M_PI;
//    }
}


void rubiks_cube::rotation_manager::try_acquire_rotation_axis(uint32_t axis)
{
    if (!m_axis_acquired.any() && !m_axis_acquired.test(axis)) {
        m_axis_acquired.set(axis);
    }
}


void rubiks_cube::rotation_manager::release_rotation_axis()
{
    uint32_t rot_axis;

    if (m_axis_acquired.any()) {
        if (m_axis_acquired.test(x_axis)) {
            rot_axis = x_axis;
        } else if (m_axis_acquired.test(y_axis)) {
            rot_axis = y_axis;
        } else if (m_axis_acquired.test(z_axis)) {
            rot_axis = z_axis;
        }
        m_axis_acquired.reset();
    }

    m_row_index = -1;
}
