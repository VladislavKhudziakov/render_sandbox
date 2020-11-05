

#include "default_camera.hpp"

#include <scene/pipelines/render_technique.hpp>


renderer::scene::default_camera::default_camera(renderer::scene::render_technique* pipeline)
    : camera(pipeline)
{
}


void renderer::scene::default_camera::draw_instances(
    renderer::scene::mesh_instance** p_instance,
    size_t instances_count)
{
    render_technique->draw_instances(p_instance, instances_count);
}

