

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

    return rot_x * rot_y * rot_z * math::translation({float(m_position.x), float(m_position.y), float(m_position.z)});
}


void rubiks_cube::cube::rotate(uint32_t axis, float angle)
{
    m_rotation[axis] += angle;
}

void rubiks_cube::cube::update_position(uint32_t axis, int direction)
{
    math::mat4 rotation;
    if (axis == x_axis) {
        rotation = math::rotation_x(M_PI_2 * direction);
    } else if (axis == y_axis) {
        rotation = math::rotation_y(M_PI_2 * direction);
    } else {
        rotation = math::rotation_z(M_PI_2 * direction);
    }

    math::vec4 t{float(m_position.x), float(m_position.y), float(m_position.z), 0.0};

    t = t * rotation;

    m_position.x = round(t.x);
    m_position.y = round(t.y);
    m_position.z = round(t.z);

    m_rotation[x_axis] = 0;
    m_rotation[y_axis] = 0;
    m_rotation[z_axis] = 0;
}


math::ivec3 rubiks_cube::cube::get_position()
{
    return m_position;
}
