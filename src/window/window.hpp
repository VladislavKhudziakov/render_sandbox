


#pragma once

#include <misc/misc.hpp>

namespace renderer
{
    class window
    {
    public:
        explicit window(misc::size);
        virtual ~window() = default;

        virtual void update() = 0;

        virtual void close() = 0;
        virtual bool closed() = 0;

        misc::size get_size();

    protected:
        misc::size m_size;
    };
} // namespace renderer
