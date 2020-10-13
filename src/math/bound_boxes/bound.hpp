

#pragma once

#include <math/vector.hpp>
#include <misc/misc.hpp>

#include <algorithm>

namespace math::bound_boxes
{
    template<size_t Size, typename DataType = float>
    class bound;

    namespace detail
    {
        template<size_t Index>
        struct less
        {
            template< size_t Size, typename DataType>
            inline static void call(math::vec<Size, DataType> l, math::vec<Size, DataType> r, bool& res)
            {
                res = res && math::detail::vector_element<Index>::get(l) < math::detail::vector_element<Index>::get(r);
            }
        };

        template<size_t Index>
        struct less_eq
        {
            template< size_t Size, typename DataType>
            inline static void call(math::vec<Size, DataType> l, math::vec<Size, DataType> r, bool& res)
            {
                res = res && math::detail::vector_element<Index>::get(l) <= math::detail::vector_element<Index>::get(r);
            }
        };

        template<size_t Index>
        struct greater
        {
            template< size_t Size, typename DataType>
            inline static void call(math::vec<Size, DataType> l, math::vec<Size, DataType> r, bool& res)
            {
                res = res && math::detail::vector_element<Index>::get(l) > math::detail::vector_element<Index>::get(r);
            }
        };

        template<size_t Index>
        struct greater_eq
        {
            template< size_t Size, typename DataType>
            inline static void call(math::vec<Size, DataType> l, math::vec<Size, DataType> r, bool& res)
            {
                res = res && math::detail::vector_element<Index>::get(l) >= math::detail::vector_element<Index>::get(r);
            }
        };

        template<size_t Index>
        struct assign_corner_val
        {
            template< size_t Size, typename DataType>
            inline static void call(const math::bound_boxes::bound<Size, DataType>& b, math::vec<Size, DataType>& v, size_t i)
            {
                auto bv = b[bool(i & (1ull << Index))];
                math::detail::vector_element<Index>::get(v) = math::detail::vector_element<Index>::get(bv);
            }
        };
    } // namespace detail


    template<size_t Size, typename DataType>
    class bound
    {
    public:
        constexpr static size_t max_corner = 1ull << Size;
        constexpr static size_t min_index = 0;
        constexpr static size_t max_index = 1;

        bound()
            : min{}
            , max{}
        {
            math::detail::transform<Size - 1>::apply(min, min, min, [](auto& v1, auto& v2) {
                return std::numeric_limits<DataType>::max();
            });
            math::detail::transform<Size - 1>::apply(max, max, max, [](auto& v1, auto& v2) {
                return std::numeric_limits<DataType>::min();
            });
        };

        bound(math::vec<Size, DataType> min, math::vec<Size, DataType> max)
            : min(min)
            , max(max)
        {
            math::detail::transform<Size - 1>::apply(min, max, min, [](auto& v1, auto& v2) {
                return std::min(v1, v2);
            });
            math::detail::transform<Size - 1>::apply(min, max, max, [](auto& v1, auto& v2) {
                return std::max(v1, v2);
            });
        }

        ~bound() = default;

        math::vec<Size, DataType> operator[](size_t i) const
        {
            ASSERT(i < 2);
            return i == 0 ? min : max;
        }

        math::vec<Size, DataType> min;
        math::vec<Size, DataType> max;
    };


    template< size_t Size, typename DataType>
    math::vec<Size, DataType> diagonal(const bound<Size, DataType>& b)
    {
        return b.max - b.min;
    }


    template< size_t Size, typename DataType>
    DataType volume(const bound<Size, DataType>& b)
    {
        auto d = diagonal(b);
        return math::detail::accumulate<Size - 1>::apply(
            d, [](auto& val) { return val; }, [](auto a, auto b) { return a * b; });
    }


    template< size_t Size, typename DataType>
    bound<Size, DataType> union_bounds(const bound<Size, DataType>& b1, const bound<Size, DataType>& b2)
    {
        bound<Size, DataType> res;
        res.min = min(b1.min, b2.min);
        res.max = max(b1.max, b2.max);
        return res;
    }


    template< size_t Size, typename DataType>
    bound<Size, DataType> union_bound_point(const bound<Size, DataType>& b1, math::vec<Size, DataType> p)
    {
        bound<Size, DataType> res;
        res.min = min(b1.min, p);
        res.max = max(b1.max, p);
        return res;
    }


    template< size_t Size, typename DataType>
    bool inside(const bound<Size, DataType>& b1, vec<Size, DataType> point)
    {
        bool less_max{true};
        bool greater_min{true};
        math::detail::for_i<Size - 1>::template call<detail::greater_eq>(point, b1.min, greater_min);
        math::detail::for_i<Size - 1>::template call<detail::less_eq>(point, b1.max, less_max);
        return greater_min && less_max;
    }


    template< size_t Size, typename DataType>
    bool inside_excl(const bound<Size, DataType>& b1, vec<Size, DataType> point)
    {
        bool less_max{true};
        bool greater_min{true};
        math::detail::for_i<Size - 1>::template call<detail::greater_eq>(point, b1.min, greater_min);
        math::detail::for_i<Size - 1>::template call<detail::less>(point, b1.max, less_max);
        return greater_min && less_max;
    }


    template< size_t Size, typename DataType>
    bool overlaps(const bound<Size, DataType>& b1, const bound<Size, DataType>& b2)
    {
        bool less_max{true};
        bool greater_min{true};
        math::detail::for_i<Size - 1>::template call<detail::greater_eq>(b1.max, b1.min, greater_min);
        math::detail::for_i<Size - 1>::template call<detail::less_eq>(b2.min, b1.max, less_max);
        return greater_min && less_max;
    }


    template< size_t Size, typename DataType>
    math::vec<Size, DataType> corner(const bound<Size, DataType>& b, size_t i)
    {
        ASSERT(i < (1ull << Size));
        math::vec<Size, DataType> res;
        math::detail::for_i<Size - 1>::template call<detail::assign_corner_val>(b, res, i);
        return res;
    }


    template< size_t Size, typename DataType>
    bound<Size, DataType> intersect(const bound<Size, DataType>& b1, const bound<Size, DataType>& b2)
    {
        bound<Size, DataType> res;
        res.min = max(b1.min, b2.min);
        res.max = min(b1.max, b2.max);
        return res;
    }

    template< size_t Size, typename DataType>
    void bound_sphere(const bound<Size, DataType>& b, vec<Size, DataType>& v, DataType& r)
    {
        v = (b.max - b.min) / 2;
        r = inside(b, v) ? length(b.max - v) : 0;
    }


    using bound4 = bound<4, float>;
    using bound3 = bound<3, float>;
    using bound2 = bound<2, float>;

    using bound4i = bound<4, int32_t>;
    using bound3i = bound<3, int32_t>;
    using bound2i = bound<2, int32_t>;
} // namespace math::bound_boxes
