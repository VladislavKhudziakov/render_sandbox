

#pragma once

namespace renderer::scene
{
    class image;

    class texture
    {
    public:
        virtual ~texture() = default;
        virtual image* get_image() = 0;
        virtual const image* get_image() const = 0;
    };
}

