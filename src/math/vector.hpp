


#pragma once

#include <cstddef>
#include <cmath>
#include <cstdint>


#if defined(__WIN32) || defined(__WIN32__) || defined(WIN32)
    #define M_E 2.71828182845904523536028747135266250        /* e              */
    #define M_LOG2E 1.44269504088896340735992468100189214    /* log2(e)        */
    #define M_LOG10E 0.434294481903251827651128918916605082  /* log10(e)       */
    #define M_LN2 0.693147180559945309417232121458176568     /* loge(2)        */
    #define M_LN10 2.30258509299404568401799145468436421     /* loge(10)       */
    #define M_PI 3.14159265358979323846264338327950288       /* pi             */
    #define M_PI_2 1.57079632679489661923132169163975144     /* pi/2           */
    #define M_PI_4 0.785398163397448309615660845819875721    /* pi/4           */
    #define M_1_PI 0.318309886183790671537767526745028724    /* 1/pi           */
    #define M_2_PI 0.636619772367581343075535053490057448    /* 2/pi           */
    #define M_2_SQRTPI 1.12837916709551257389615890312154517 /* 2/sqrt(pi)     */
    #define M_SQRT2 1.41421356237309504880168872420969808    /* sqrt(2)        */
    #define M_SQRT1_2 0.707106781186547524400844362104849039 /* 1/sqrt(2)      */
#endif

namespace math
{
    template<size_t size, typename DataType>
    struct vec;

    namespace detail
    {
        template<size_t index>
        struct vector_element;

        template<>
        struct vector_element<0>
        {
            template<size_t VectorSize, typename DataType>
            inline static DataType& get(vec<VectorSize, DataType>& v)
            {
                return v.x;
            }
        };


        template<>
        struct vector_element<1>
        {
            template<size_t VectorSize, typename DataType>
            inline static DataType& get(vec<VectorSize, DataType>& v)
            {
                return v.y;
            }
        };


        template<>
        struct vector_element<2>
        {
            template<size_t VectorSize, typename DataType>
            inline static DataType& get(vec<VectorSize, DataType>& v)
            {
                return v.z;
            }
        };


        template<>
        struct vector_element<3>
        {
            template<size_t VectorSize, typename DataType>
            inline static DataType& get(vec<VectorSize, DataType>& v)
            {
                return v.w;
            }
        };


        template<size_t i>
        struct for_i
        {
            template<template<size_t> typename MetaFunc, typename... Args>
            inline static void call(Args&&... args)
            {
                MetaFunc<i>::call(std::forward<Args>(args)...);
                if constexpr (i != 0) {
                    for_i<i - 1>::template call<MetaFunc>(std::forward<Args>(args)...);
                }
            }
        };


        template<size_t index>
        struct transform
        {
            template<size_t VectorSize, typename DataType, typename Functional>
            inline static void apply(vec<VectorSize, DataType>& v1, vec<VectorSize, DataType>& v2, vec<VectorSize, DataType>& res, Functional f)
            {
                if constexpr (index == 0) {
                    vector_element<0>::get(res) = f(vector_element<0>::get(v1), vector_element<0>::get(v2));
                } else {
                    vector_element<index>::get(res) = f(vector_element<index>::get(v1), vector_element<index>::get(v2));
                    transform<index - 1>::apply(v1, v2, res, std::move(f));
                }
            }
        };


        template<size_t index>
        struct accumulate
        {
            template<size_t VectorSize, typename DataType, typename Functional>
            inline static decltype(auto) apply(vec<VectorSize, DataType>& v, Functional f)
            {
                if constexpr (index == 0) {
                    return f(vector_element<0>::get(v));
                } else {
                    return f(vector_element<index>::get(v)) + accumulate<index - 1>::apply(v, std::move(f));
                }
            }
        };


        template<size_t index>
        struct dot_product
        {
            template<template<size_t> typename LhsGet, template<size_t> typename RhsGet, typename LhsType, typename RhsType>
            inline static auto calculate(LhsType&& lhs, RhsType&& rhs)
            {
                if constexpr (index == 0) {
                    return LhsGet<0>::get(lhs) * RhsGet<0>::get(rhs);
                } else {
                    return LhsGet<index>::get(lhs) * RhsGet<index>::get(rhs) + dot_product<index - 1>::template calculate<LhsGet, RhsGet>(lhs, rhs);
                }
            }
        };

        template<size_t index>
        struct vectors_equality
        {
            template<typename DataType, size_t Size>
            inline static bool equals(vec<Size, DataType>& v1, vec<Size, DataType>& v2)
            {
                static_assert(index <= Size - 1);
                if constexpr (index == 0) {
                    return vector_element<0>::get(v1) == vector_element<0>::get(v2);
                } else {
                    return vector_element<index>::get(v1) == vector_element<index>::get(v2) && vectors_equality<index - 1>::equals(v1, v2);
                }
            }
        };
    } // namespace detail


    template<typename DataType>
    struct vec<2, DataType>
    {
        DataType x;
        DataType y;
    };


    template<typename DataType>
    struct vec<3, DataType>
    {
        DataType x;
        DataType y;
        DataType z;
    };


    template<typename DataType>
    struct vec<4, DataType>
    {
        DataType x;
        DataType y;
        DataType z;
        DataType w;
    };


    template<typename DataType, size_t size>
    DataType dot(vec<size, DataType> lhs, vec<size, DataType> rhs)
    {
        return detail::dot_product<size - 1>::template calculate<detail::vector_element, detail::vector_element>(lhs, rhs);
    }

    template<typename DataType>
    vec<3, DataType> cross(vec<3, DataType> x, vec<3, DataType> y)
    {
        return {x.y * y.z - y.y * x.z, x.z * y.x - y.z * x.x, x.x * y.y - y.x * x.y};
    }


    template<typename DataType, size_t size>
    DataType length(vec<size, DataType> vector)
    {
        return ::sqrt(detail::accumulate<size - 1>::apply(vector, [](auto& res) {
            return res * res;
        }));
    }


    template<typename DataType, size_t size>
    vec<size, DataType> normalize(vec<size, DataType> vector)
    {
        auto l = length(vector);

        vec<size, DataType> res;

        detail::transform<size - 1>::apply(vector, res, res, [&l](auto& v1, auto& v2) {
            return v1 / l;
        });

        return res;
    }


    template<typename DataType, size_t size>
    void operator+=(vec<size, DataType>& v1, vec<size, DataType> v2)
    {
        detail::transform<size - 1>::apply(v1, v2, v1, [](auto& val1, auto& val2) {
            return val1 + val2;
        });
    }


    template<typename DataType, size_t size>
    vec<size, DataType> operator+(vec<size, DataType> v1, vec<size, DataType> v2)
    {
        v1 += v2;
        return v1;
    }


    template<typename DataType, typename ScalarType, size_t size>
    void operator+=(vec<size, DataType>& v1, ScalarType scalar)
    {
        detail::transform<size - 1>::apply(v1, v1, v1, [&scalar](auto& val1, auto& val2) {
            return val1 + scalar;
        });
    }


    template<typename DataType, typename ScalarType, size_t size>
    vec<size, DataType> operator+(vec<size, DataType> v1, ScalarType scalar)
    {
        v1 += scalar;
        return v1;
    }


    template<typename DataType, typename ScalarType, size_t size>
    vec<size, DataType> operator+(ScalarType scalar, vec<size, DataType> v1)
    {
        return v1 + scalar;
    }


    template<typename DataType, size_t size>
    void operator-=(vec<size, DataType>& v1, vec<size, DataType> v2)
    {
        detail::transform<size - 1>::apply(v1, v2, v1, [](auto& val1, auto& val2) {
            return val1 - val2;
        });
    }


    template<typename DataType, size_t size>
    vec<size, DataType> operator-(vec<size, DataType> v1, vec<size, DataType> v2)
    {
        v1 -= v2;
        return v1;
    }


    template<typename DataType, typename ScalarType, size_t size>
    void operator-=(vec<size, DataType>& v1, ScalarType scalar)
    {
        detail::transform<size - 1>::apply(v1, v1, v1, [&scalar](auto& val1, auto& val2) {
            return val1 - scalar;
        });
    }


    template<typename DataType, typename ScalarType, size_t size>
    vec<size, DataType> operator-(vec<size, DataType> v1, ScalarType scalar)
    {
        v1 -= scalar;
        return v1;
    }


    template<typename DataType, typename ScalarType, size_t size>
    vec<size, DataType> operator-(ScalarType scalar, vec<size, DataType> v1)
    {
        detail::transform<size - 1>::apply(v1, v1, v1, [&scalar](auto& val1, auto& val2) {
            return scalar - val1;
        });

        return v1;
    }


    template<typename DataType, size_t size>
    vec<size, DataType> operator-(vec<size, DataType> v1)
    {
        detail::transform<size - 1>::apply(v1, v1, v1, [](auto& val1, auto& val2) {
            return -val1;
        });

        return v1;
    }


    template<typename DataType, size_t size>
    void operator/=(vec<size, DataType>& v1, vec<size, DataType> v2)
    {
        detail::transform<size - 1>::apply(v1, v2, v1, [](auto& val1, auto& val2) {
            return val1 / val2;
        });
    }


    template<typename DataType, size_t size>
    vec<size, DataType> operator/(vec<size, DataType> v1, vec<size, DataType> v2)
    {
        v1 /= v2;
        return v1;
    }


    template<typename DataType, typename ScalarType, size_t size>
    void operator/=(vec<size, DataType>& v1, ScalarType scalar)
    {
        detail::transform<size - 1>::apply(v1, v1, v1, [&scalar](auto& val1, auto& val2) {
            return val1 / scalar;
        });
    }


    template<typename DataType, typename ScalarType, size_t size>
    vec<size, DataType> operator/(vec<size, DataType> v1, ScalarType scalar)
    {
        v1 /= scalar;
        return v1;
    }


    template<typename DataType, typename ScalarType, size_t size>
    vec<size, DataType> operator/(ScalarType scalar, vec<size, DataType> v1)
    {
        detail::transform<size - 1>::apply(v1, v1, v1, [&scalar](auto& val1, auto& val2) {
            return scalar / val1;
        });
        return v1;
    }


    template<typename DataType, size_t size>
    void operator*=(vec<size, DataType>& v1, vec<size, DataType> v2)
    {
        detail::transform<size - 1>::apply(v1, v2, v1, [](auto& val1, auto& val2) {
            return val1 * val2;
        });
    }


    template<typename DataType, size_t size>
    vec<size, DataType> operator*(vec<size, DataType> v1, vec<size, DataType> v2)
    {
        v1 *= v2;
        return v1;
    }


    template<typename DataType, typename ScalarType, size_t size>
    void operator*=(vec<size, DataType>& v1, ScalarType scalar)
    {
        detail::transform<size - 1>::apply(v1, v1, v1, [&scalar](auto& val1, auto& val2) {
            return val1 * scalar;
        });
    }


    template<typename DataType, typename ScalarType, size_t size>
    vec<size, DataType> operator*(vec<size, DataType> v1, ScalarType scalar)
    {
        v1 *= scalar;
        return v1;
    }


    template<typename DataType, typename ScalarType, size_t size>
    vec<size, DataType> operator*(ScalarType scalar, vec<size, DataType> v1)
    {
        return v1 * scalar;
    }


    template<typename DataType, size_t size>
    bool operator==(vec<size, DataType> v1, vec<size, DataType> v2)
    {
        return detail::vectors_equality<size - 1>::equals(v1, v2);
    }

    template<typename DataType, size_t size>
    bool operator!=(vec<size, DataType> v1, vec<size, DataType> v2)
    {
        return !(v1 == v2);
    }


    template<typename DataType, size_t size>
    DataType* value_ptr(vec<size, DataType> v)
    {
        return &v.x;
    }


    using vec4 = vec<4, float>;
    using vec3 = vec<3, float>;
    using vec2 = vec<2, float>;

    using dvec4 = vec<4, double>;
    using dvec3 = vec<3, double>;
    using dvec2 = vec<2, double>;

    using ivec4 = vec<4, int>;
    using ivec3 = vec<3, int>;
    using ivec2 = vec<2, int>;

    using ubvec4 = vec<4, uint8_t>;
    using ubvec3 = vec<3, uint8_t>;
    using ubvec2 = vec<2, uint8_t>;


    static_assert(sizeof(vec4) == sizeof(float[4]));
    static_assert(sizeof(vec3) == sizeof(float[3]));
    static_assert(sizeof(vec2) == sizeof(float[2]));
} // namespace math
