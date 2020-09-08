


#pragma once

#include <math/vector.hpp>

#include <cstddef>
#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <array>
#include <iostream>

namespace math
{
    template<size_t Rows, size_t Cols, typename DataType>
    struct mat;

    namespace detail
    {
        template<typename T>
        struct is_matrix_or_vector : std::false_type
        {
        };

        template<size_t Rows, size_t Cols, typename DataType>
        struct is_matrix_or_vector<mat<Rows, Cols, DataType>> : public std::true_type
        {
        };

        template<size_t Rows, typename DataType>
        struct is_matrix_or_vector<vec<Rows, DataType>> : public std::true_type
        {
        };

        template<typename T>
        struct is_matrix : std::false_type
        {
        };

        template<size_t Rows, size_t Cols, typename DataType>
        struct is_matrix<mat<Rows, Cols, DataType>> : public std::true_type
        {
        };

        template<typename X, typename Y>
        struct add_result
        {
            using type = std::decay_t<decltype(std::declval<X>() + std::declval<Y>())>;
        };

        template<typename X, typename Y>
        using add_result_t = typename add_result<X, Y>::type;

        template<typename X, typename Y>
        struct sub_result
        {
            using type = std::decay_t<decltype(std::declval<X>() - std::declval<Y>())>;
        };

        template<typename X, typename Y>
        using sub_result_t = typename sub_result<X, Y>::type;

        template<typename X, typename Y>
        struct mul_result
        {
            using type = std::decay_t<decltype(std::declval<X>() * std::declval<Y>())>;
        };

        template<typename X, typename Y>
        using mul_result_t = typename mul_result<X, Y>::type;

        template<typename X, typename Y>
        struct div_result
        {
            using type = std::decay_t<decltype(std::declval<X>() / std::declval<Y>())>;
        };

        template<typename X, typename Y>
        using div_result_t = typename div_result<X, Y>::type;

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


        template<size_t Col>
        struct matrix_col
        {
            template<size_t index>
            struct element
            {
                template<size_t Rows, size_t Cols, typename DataType>
                inline static auto get(const mat<Rows, Cols, DataType>& m)
                {
                    return m[index][Col];
                }
            };
        };


        template<size_t Row>
        struct matrix_row
        {
            template<size_t Index>
            struct element
            {
                template<size_t Rows, size_t Cols, typename DataType>
                inline static auto get(const mat<Rows, Cols, DataType>& m)
                {
                    return m[Row][Index];
                }

                template<
                    size_t Rows,
                    size_t Cols,
                    typename DataType,
                    typename Functional,
                    typename... Args>
                inline static void call(mat<Rows, Cols, DataType>& m, Functional f, Args&&... args)
                {
                    f(Row, Index, std::forward<Args>(args)...);
                }
            };
        };


        template<size_t ColIndex>
        struct calc_col
        {
            template<size_t RowIndex>
            struct elem
            {
                template<
                    size_t RowsX,
                    size_t ColsX,
                    size_t RowsY,
                    size_t ColsY,
                    typename ResType,
                    typename XDataType,
                    typename YDataType>
                inline static void call(
                    mat<RowsX, ColsY, ResType>& res,
                    const mat<RowsX, ColsX, XDataType>& x,
                    const mat<RowsY, ColsY, YDataType>& y)
                {
                    res[RowIndex][ColIndex] = dot_product<RowsY - 1>::template calculate<
                        matrix_row<RowIndex>::template element,
                        matrix_col<ColIndex>::template element>(x, y);
                }
            };
        };


        template<size_t ColIndex>
        struct calc_row
        {
            template<
                size_t RowsX,
                size_t ColsX,
                size_t RowsY,
                size_t ColsY,
                typename ResType,
                typename XDataType,
                typename YDataType>
            inline static void call(
                mat<RowsX, ColsY, ResType>& res,
                const mat<RowsX, ColsX, XDataType>& x,
                const mat<RowsY, ColsY, YDataType>& y)
            {
                static_assert(ColIndex < ColsY);
                for_i<RowsX - 1>::template call<calc_col<ColIndex>::template elem>(res, x, y);
            }
        };


        template<size_t Row>
        struct mat_mul_vec
        {
            template<size_t Cols, size_t Rows, typename ResType, typename VectorType, typename MatrixDataType>
            inline static void call(vec<Rows, ResType>& res, vec<Rows, VectorType>& v, mat<Cols, Rows, MatrixDataType>& m)
            {
                vector_element<Row>::get(res) =
                    dot_product<Rows - 1>::template calculate<
                        vector_element,
                        matrix_row<Row>::template element>(v, m);
            }
        };


        template<size_t Col>
        struct vec_mul_mat
        {
            template<size_t Cols, size_t Rows, typename ResType, typename VectorType, typename MatrixDataType>
            inline static void call(vec<Rows, ResType>& res, vec<Rows, VectorType>& v, mat<Cols, Rows, MatrixDataType>& m)
            {
                vector_element<Col>::get(res) =
                    dot_product<Cols - 1>::template calculate<
                        matrix_col<Col>::template element,
                        vector_element>(m, v);
            }
        };


        template<size_t Row>
        struct for_each
        {
            template<
                size_t Rows,
                size_t Cols,
                typename DataType,
                typename Functional,
                typename... Args>
            inline static void call(mat<Rows, Cols, DataType>& m, Functional f, Args&&... args)
            {
            }
        };


        template<size_t Row>
        struct for_each_row
        {
            template<
                size_t Rows,
                size_t Cols,
                typename DataType,
                typename Functional,
                typename... Args>
            inline static void call(mat<Rows, Cols, DataType>& m, Functional f, Args&&... args)
            {
                for_i<Cols - 1>::template call<matrix_row<Row>::template element>(m, std::move(f), std::forward<Args>(args)...);
            }
        };

        template<
            size_t Rows,
            size_t Cols,
            typename DataType,
            typename Functional,
            typename... Args>
        inline void for_each_matrix_element(mat<Rows, Cols, DataType>& m, Functional f, Args&&... args)
        {
            for_i<Rows - 1>::template call<for_each_row>(m, std::move(f), std::forward<Args>(args)...);
        }
    } // namespace detail


    template<size_t Cols, size_t Rows, typename DataType>
    struct mat
    {
    public:
        mat()
        {
            for (size_t row = 0; row < Rows; row++) {
                for (size_t col = 0; col < Cols; col++) {
                    m_elements[row][col] = row == col ? 1 : 0;
                }
            }
        }


        mat(std::initializer_list<DataType> init_values)
        {
            for (size_t row = 0; row < Rows; row++) {
                for (size_t col = 0; col < Cols; col++) {
                    auto index = col + row * Rows;

                    if (index >= init_values.size()) {
                        m_elements[row][col] = row == col ? 1 : 0;
                        continue;
                    }

                    m_elements[row][col] = *(init_values.begin() + index);
                }
            }
        }

        auto& operator[](size_t index)
        {
            assert(index < Rows);
            return m_elements[index];
        }

        const auto& operator[](size_t index) const
        {
            assert(index < Rows);
            return m_elements[index];
        }


        friend DataType* value_ptr(mat& m)
        {
            return m.m_elements[0];
        }

    private:
        DataType m_elements[Rows][Cols];
    };


    template<size_t RowsX, size_t ColsX, typename DataTypeX, size_t RowsY, size_t ColsY, typename DataTypeY>
    auto operator*(const mat<RowsX, ColsX, DataTypeX>& x, const mat<RowsY, ColsY, DataTypeY>& y)
    {
        static_assert(ColsX == RowsY);
        mat<RowsX, ColsY, detail::mul_result_t<DataTypeX, DataTypeY>> res;
        detail::for_i<ColsY - 1>::template call<detail::calc_row>(res, x, y);
        return res;
    }


    template<size_t MatCols, size_t MatRows, typename MatDataType, typename VecDataType>
    auto operator*(mat<MatRows, MatCols, MatDataType>& mat, vec<MatCols, VecDataType>& v)
    {
        vec<MatCols, detail::mul_result_t<MatDataType, VecDataType>> res;
        detail::for_i<MatCols - 1>::template call<detail::mat_mul_vec>(res, v, mat);
        return res;
    }


    template<size_t MatCols, size_t MatRows, typename MatDataType, typename VecDataType>
    auto operator*(vec<MatCols, VecDataType>& v, mat<MatRows, MatCols, MatDataType>& mat)
    {
        vec<MatCols, detail::mul_result_t<MatDataType, VecDataType>> res;
        detail::for_i<MatRows - 1>::template call<detail::vec_mul_mat>(res, v, mat);
        return res;
    }


    template<
        size_t MatCols,
        size_t MatRows,
        typename MatDataType,
        typename ScalarType,
        std::enable_if<!detail::is_matrix_or_vector<ScalarType>::value>>
    auto operator*(
        mat<MatCols, MatRows, MatDataType> m,
        ScalarType&& scalar)
    {
        detail::for_each_matrix_element(
            m,
            [](size_t row, size_t col, auto& m, auto&& scalar) { m[row][col] *= scalar; },
            m,
            scalar);

        return m;
    }


    template<size_t MatCols, size_t MatRows, typename MatDataType, typename ScalarType>
    auto operator/(mat<MatCols, MatRows, MatDataType> m, ScalarType scalar)
    {
        detail::for_each_matrix_element(
            m, [](size_t row, size_t col, auto& m, auto scalar) { m[row][col] /= scalar; }, m, scalar);
        return m;
    }


    template<size_t MatCols, size_t MatRows, typename DataTypeX, typename DataTypeY>
    auto operator+(mat<MatCols, MatRows, DataTypeX> x, mat<MatCols, MatRows, DataTypeY>& y)
    {
        detail::for_each_matrix_element(
            x, [](size_t row, size_t col, auto& x, auto& y) { x[row][col] += y[row][col]; }, x, y);
        return x;
    }


    template<size_t MatCols, size_t MatRows, typename DataTypeX, typename DataTypeY>
    auto operator-(mat<MatCols, MatRows, DataTypeX> x, mat<MatCols, MatRows, DataTypeY>& y)
    {
        detail::for_each_matrix_element(
            x, [](size_t row, size_t col, auto& x, auto& y) { return x[row][col] -= y[row][col]; }, x, y);
        return x;
    }


    template<size_t MatCols, size_t MatRows, typename MatDataType, typename ScalarType>
    auto operator+(mat<MatCols, MatRows, MatDataType> m, ScalarType scalar)
    {
        detail::for_each_matrix_element(
            m, [](size_t row, size_t col, auto& m, auto scalar) { return m[row][col] += scalar; }, m, scalar);
        return m;
    }


    template<size_t MatCols, size_t MatRows, typename MatDataType, typename ScalarType>
    auto operator-(mat<MatCols, MatRows, MatDataType> m, ScalarType scalar)
    {
        detail::for_each_matrix_element(
            m, [](size_t row, size_t col, auto& m, auto scalar) { return m[row][col] -= scalar; }, m, scalar);
        return m;
    }


    using mat4 = mat<4, 4, float>;
    using mat3 = mat<3, 3, float>;
    using mat2 = mat<2, 2, float>;


    inline mat4 rotation_z(float angle)
    {
        float s = sin(angle);
        float c = cos(angle);

        mat4 mat{};

        mat[0][0] = c;
        mat[0][1] = s;
        mat[1][0] = -s;
        mat[1][1] = c;

        return mat;
    }

    inline mat4 rotation_x(float angle)
    {
        float s = sin(angle);
        float c = cos(angle);

        mat4 mat{};

        mat[1][1] = c;
        mat[1][2] = s;
        mat[2][1] = -s;
        mat[2][2] = c;

        return mat;
    }

    inline mat4 rotation_y(float angle)
    {
        float s = sin(angle);
        float c = cos(angle);

        mat4 mat{
            c,  0, -s,  0,
            0,  1,  0,  0,
            s,  0,  c,  0,
            0,  0,  0,  1
        };

//        mat[0][0] = c;
//        mat[0][2] = -s;
//        mat[2][0] = s;
//        mat[0][2] = c;


        return mat;
    }

    inline mat4 scale(vec3 s)
    {
        mat4 mat;
        mat[0][0] = s.x;
        mat[1][1] = s.y;
        mat[2][2] = s.z;
        return mat;
    }

    inline mat4 translation(vec3 t)
    {
        mat4 mat;
        mat[3][0] = t.x;
        mat[3][1] = t.y;
        mat[3][2] = t.z;
        return mat;
    }


    inline mat4 look_at(vec3 eye, vec3 target, vec3 up_dir)
    {
        mat4 mat;

        eye.x = -eye.x;
        eye.y = -eye.y;

        vec3 fwd = normalize(target - eye);
        auto side = normalize(cross(fwd, up_dir));
        auto up = normalize(cross(side, fwd));

        mat[0][0] = side.x;
        mat[0][1] = side.y;
        mat[0][2] = side.z;

        mat[1][0] = up.x;
        mat[1][1] = up.y;
        mat[1][2] = up.z;

        mat[2][0] = -fwd.x;
        mat[2][1] = -fwd.y;
        mat[2][2] = -fwd.z;

        mat[3][0] = -dot(side, eye);
        mat[3][1] = -dot(up, eye);
        mat[3][2] = dot(fwd, eye);

        return mat;
    }


    inline mat4 perspective(float fov, float near, float far, float width, float height)
    {
        float h = cosf(fov * 0.5f) / sinf(fov * 0.5f);
        float w = h * (height / width);

        mat4 mat;

        mat[0][0] = w;
        mat[1][1] = h;
        mat[2][2] = -(far + near) / (far - near);
        mat[2][3] = -1;
        mat[3][2] = -(near * far * 2) / (far - near);
        mat[3][3] = 0;

        return mat;
    }

    inline float radians(float degs)
    {
        return degs * float(M_PI) / 180.f;
    }


    static_assert(sizeof(mat4) == sizeof(float[4 * 4]));
    static_assert(sizeof(mat3) == sizeof(float[3 * 3]));
    static_assert(sizeof(mat2) == sizeof(float[2 * 2]));
} // namespace math
