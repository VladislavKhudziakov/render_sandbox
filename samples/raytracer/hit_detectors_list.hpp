


#pragma once

#include <hit_detector.hpp>

#include <vector>
#include <memory>


namespace raytracer
{
    class hit_detectors_list : public raytracer::hit_detector
    {
    public:
        ~hit_detectors_list() override = default;

        template<typename T, typename... Args>
        void add_detector(Args&&... args)
        {
            m_hit_detectors.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        bool hit(math::raytracing::ray3 ray, raytracer::hit_record& record, float t_min, float t_max) override
        {
            raytracer::hit_record closest{};
            bool is_first = true;
            bool hit_success = false;

            for (auto& detector : m_hit_detectors) {
                raytracer::hit_record curr_record{};
                if (detector->hit(ray, curr_record, t_min, t_max)) {
                    if (is_first) {
                        closest = curr_record;
                        hit_success = true;
                        is_first = false;
                        continue;
                    }

                    if (curr_record.t < closest.t) {
                        closest = curr_record;
                    }
                }
            }

            record = closest;

            return hit_success;
        }

    private:
        std::vector<std::unique_ptr<hit_detector>> m_hit_detectors;
    };
}
