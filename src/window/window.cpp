

#include "window.hpp"


renderer::window::window(misc::size size)
: m_size(size)
{
}


misc::size renderer::window::get_size()
{
    return m_size;
}
