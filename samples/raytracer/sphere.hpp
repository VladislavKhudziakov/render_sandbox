


#pragma once

#include <hit_detector.hpp>
#include <material.hpp>

#include <memory>

namespace raytracer
{
    class sphere : public raytracer::hit_detector
    {
    public:
        sphere(math::vec3 o, float r, std::unique_ptr<material> material = nullptr);

        virtual ~sphere() = default;

        bool hit(math::raytracing::ray ray, raytracer::hit_record& record, float t_min, float t_max) override;

        math::vec3 origin;
        float radius;

    private:
        std::unique_ptr<material> m_material;
    };
}
