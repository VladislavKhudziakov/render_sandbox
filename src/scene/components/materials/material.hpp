

#pragma once

#include <scene/components/component_base.hpp>

#include <vector>

namespace renderer::scene
{
    class texture;
    class shader;

    class material : public component_base
    {
    public:
        material() = default;
        explicit material(shader*);
        ~material() override = default;

        virtual void set_sampler(const std::string&, texture*) = 0;
        virtual texture* get_sampler(const std::string&) = 0;
        virtual const texture* get_sampler(const std::string&) const = 0;

        shader* shader{nullptr};
    };
}

