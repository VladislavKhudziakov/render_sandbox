

#include "metal.hpp"

#include <math/misc/misc.hpp>


bool raytracer::metal::scatter(
    math::raytracing::ray3 ray,
    const raytracer::hit_record& record,
    math::vec3& attenuation,
    math::raytracing::ray3& scattered)
{
    auto r = math::misc::random_in_unit_sphere();
    r *= 1. - m_metalness;
    scattered = {record.point, math::misc::reflect(ray.direction + r, record.normal)};
    attenuation = m_albedo;
    return math::dot(scattered.direction, record.normal) > 0.000001;
}


raytracer::metal::metal(math::vec3 albedo, float metalness)
    : m_albedo(albedo)
    , m_metalness(metalness)
{
}
