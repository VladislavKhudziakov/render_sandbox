

#include "camera.hpp"

#include <math/matrix_operations.hpp>


void renderer::camera::update()
{
    m_view = math::look_at(position, target_position, {0, 1, 0});
    m_proj = math::perspective(fov, near, far, width, height);
    m_transformation = m_proj * m_view;
}


const math::mat4& renderer::camera::get_transformation()
{
    return m_transformation;
}


const math::mat4& renderer::camera::get_proj() const
{
    return m_proj;
}


const math::mat4& renderer::camera::get_view() const
{
    return m_view;
}
