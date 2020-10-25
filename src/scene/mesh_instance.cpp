

#include "mesh_instance.hpp"

#include <renderer/renderer.hpp>
#include <shapes/shape.hpp>

renderer::scene::mesh_instance::mesh_instance(::renderer::renderer* r, std::unique_ptr<shapes::shape> shape)
    : m_renderer(r)
    , m_shape(std::move(shape))
{
    ::renderer::mesh_layout_descriptor mld {
        .vertex_attributes = {
            {.data_type = ::renderer::data_type::f32, .elements_count = 3},
            {.data_type = ::renderer::data_type::f32, .elements_count = 2},
            {.data_type = ::renderer::data_type::f32, .elements_count = 3},
            {.data_type = ::renderer::data_type::f32, .elements_count = 3}},
        .topology = ::renderer::geometry_topology::triangles
    };

    m_shape->generate(mld.vertex_data,mld.index_data);

    m_handler = r->create_mesh(mld);
}


uint32_t renderer::scene::mesh_instance::get_handler() const
{
    return m_handler;
}


renderer::scene::mesh_instance::~mesh_instance() = default;