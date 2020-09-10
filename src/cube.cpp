

#include "cube.hpp"



rubiks_cube::cube::cube(math::ivec3 translation)
: m_position(translation)
{
}


math::mat4 rubiks_cube::cube::get_transformation()
{
    auto rot_x = math::rotation_x(m_rotation[x_axis]);
    auto rot_y = math::rotation_y(m_rotation[y_axis]);
    auto rot_z = math::rotation_z(m_rotation[z_axis]);

    return math::translation({float(m_position.x), float(m_position.y), float(m_position.z)}) * rot_x * rot_y * rot_z;
}


void rubiks_cube::cube::rotate(uint32_t axis, float angle)
{
    m_rotation[axis] = +angle;
}

void rubiks_cube::cube::update_position()
{
    auto rot_x = math::rotation_x(m_rotation[x_axis]);
    auto rot_y = math::rotation_y(m_rotation[y_axis]);
    auto rot_z = math::rotation_z(m_rotation[z_axis]);

    auto rotation = rot_x * rot_y * rot_z;

    math::vec4 t{float(m_position.x), float(m_position.y), float(m_position.z), 1.0};

    t = rotation * t;

    m_position.x = t.x;
    m_position.y = t.y;
    m_position.z = t.z;

    m_rotation[x_axis] = 0;
    m_rotation[y_axis] = 0;
    m_rotation[z_axis] = 0;
}


math::ivec3 rubiks_cube::cube::get_position()
{
    return m_position;
}
