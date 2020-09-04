

#include "window.hpp"


renderer::window::window(misc::size size, std::unique_ptr<::renderer::renderer> renderer)
    : m_size(size)
    , m_renderer(std::move(renderer))
{
}


misc::size renderer::window::get_size()
{
    return m_size;
}


::renderer::renderer* renderer::window::get_renderer()
{
    return m_renderer.get();
}
