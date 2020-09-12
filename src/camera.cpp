

#include "camera.hpp"


void renderer::camera::update()
{
    auto view = math::look_at(position, target_position, {0, 1, 0});
    auto proj = math::perspective(fov, near, far, width, height);
    m_transformation =  proj * view;
}


const math::mat4& renderer::camera::get_transformation()
{
    return m_transformation;
}
