


#pragma once

#include <misc/misc.hpp>

#include <renderer/renderer.hpp>

namespace renderer
{
    class window
    {
    public:
        explicit window(misc::size, std::unique_ptr<::renderer::renderer>);
        virtual ~window() = default;

        virtual void update() = 0;

        virtual void close() = 0;
        virtual bool closed() = 0;

        misc::size get_size();
        ::renderer::renderer* get_renderer();

    protected:
        misc::size m_size;
        std::unique_ptr<::renderer::renderer> m_renderer;
    };
} // namespace renderer
