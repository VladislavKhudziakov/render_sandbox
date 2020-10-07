

#include "dielectric.hpp"

#include <math/vector.hpp>
#include <math/misc/misc.hpp>


raytracer::dielectric::dielectric(math::vec3 albedo, float ref_c)
    : m_albedo(albedo)
    , m_refraction_coefficient(ref_c)
{
}


bool raytracer::dielectric::scatter(
    math::raytracing::ray ray,
    const raytracer::hit_record& record,
    math::vec3& attenuation,
    math::raytracing::ray& scattered)
{
    math::vec3 on;
    float ni_over_nt;

    if (math::dot(ray.direction, record.normal) > 0) {
        on = -record.normal;
        ni_over_nt = m_refraction_coefficient;
    } else {
        on = record.normal;
        ni_over_nt = 1.f / m_refraction_coefficient;
    }

    math::vec3 res;

    attenuation = m_albedo;

    if (math::misc::refract(ray.direction, on, res, ni_over_nt)) {
        scattered = {record.point, res};
        return true;
    } else {
        scattered = {record.point, math::misc::reflect(ray.direction, record.normal)};
        return false;
    }
}
