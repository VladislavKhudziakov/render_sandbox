

#include "lambertian.hpp"

#include <math/misc/misc.hpp>

raytracer::lambertian::lambertian(math::vec3 albedo)
    : albedo(albedo)
{
}


bool raytracer::lambertian::scatter(
    math::raytracing::ray ray,
    const raytracer::hit_record& record,
    math::vec3& attenuation,
    math::raytracing::ray& scattered)
{
    auto dir = math::normalize(record.normal + math::misc::random_in_unit_sphere());
    scattered = {record.point, dir};
    attenuation = albedo;

    return true;
}

