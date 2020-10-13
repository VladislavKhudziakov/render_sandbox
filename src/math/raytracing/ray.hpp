


#pragma once

#include <math/vector.hpp>

#include <math/bound_boxes/bound.hpp>

namespace math::raytracing
{
    template<size_t Size, typename DataType = float>
    struct ray;

    namespace detail
    {
        template<size_t Index>
        struct aabb_intersect_resolver
        {
            template<size_t Size, typename DataType>
            inline static void call(
                const raytracing::ray<Size, DataType>& r,
                const bound_boxes::bound<Size, DataType>& b,
                bool& success,
                DataType& tmin,
                DataType& tmax)
            {
                if (success == false) {
                    return;
                }

                const auto one_over_d = static_cast<DataType>(1) / math::detail::vector_element<Index>::get(r.direction);

                DataType tnear = (math::detail::vector_element<Index>::get(b.min) - math::detail::vector_element<Index>::get(r.origin)) * one_over_d;
                DataType tfar = (math::detail::vector_element<Index>::get(b.max) - math::detail::vector_element<Index>::get(r.origin)) * one_over_d;

                if (tnear > tfar) {
                    std::swap(tnear, tfar);
                }

                tmin = tnear > tmin ? tnear : tmin;
                tmax = tfar < tmax ? tfar : tmax;

                success = tmin < tmax;
            }
        };
    } // namespace detail


    template<size_t Size, typename DataType>
    struct ray
    {
        vec<Size, DataType> origin;
        vec<Size, DataType> direction;

        vec<Size, DataType> operator()(float t) const
        {
            return origin + t * direction;
        }
    };

    template<typename DataType, size_t Size>
    bool intersect(
        const ray<Size, DataType>& ray,
        const bound_boxes::bound<Size, DataType> bound,
        DataType* tmin = nullptr,
        DataType* tmax = nullptr,
        DataType max = std::numeric_limits<DataType>::max())
    {
        bool success = true;
        auto tmin_ = static_cast<DataType>(0);
        auto tmax_ = max;
        math::detail::for_i<Size - 1>::template call<detail::aabb_intersect_resolver>(ray, bound, success, tmin_, tmax_);
        if (tmin) {
            *tmin = tmin_;
        }
        if (tmax) {
            *tmax = tmax_;
        }

        return success;
    }


    using ray4 = ray<4, float>;
    using ray3 = ray<3, float>;
    using ray2 = ray<2, float>;
} // namespace math::raytracing
