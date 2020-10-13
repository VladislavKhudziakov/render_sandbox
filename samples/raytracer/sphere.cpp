

#include "sphere.hpp"
raytracer::sphere::sphere(math::vec3 o, float r, std::unique_ptr<material> material)
    : origin(o)
    , radius(r)
    , m_material(std::move(material))
{
}


bool raytracer::sphere::hit(math::raytracing::ray3 ray, raytracer::hit_record& record, float t_min, float t_max)
{
    math::vec3 oc = ray.origin - origin;
    float a = math::dot(ray.direction, ray.direction);
    float b = 2.f * math::dot(oc, ray.direction);
    float c = math::dot(oc, oc) - radius * radius;
    float d = b * b - 4.f * a * c;

    auto calc_hit_record = [this, &record, &ray](float t) {
      record.point = ray.origin + ray.direction * t;
      record.normal = math::normalize(record.point - origin);
      record.t = t;
      record.material = m_material.get();
    };

    if (d >= 0.f) {
        float t = (-b - sqrt(d)) / (2.f * a);
        if (t >= t_min && t <= t_max) {
            calc_hit_record(t);
            return true;
        }
        t = (-b + sqrt(d)) / (2.f * a);
        if (t >= t_min && t <= t_max) {
            calc_hit_record(t);
            return true;
        }
    }
    return false;
}