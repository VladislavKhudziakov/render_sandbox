

#pragma once

#include <math/matrix.hpp>


namespace math
{
    inline mat4 rotation_z(float angle)
    {
        float s = sin(angle);
        float c = cos(angle);

        mat4 mat{};

        mat[0][0] = c;
        mat[0][1] = -s;
        mat[1][0] = s;
        mat[1][1] = c;
        mat[2][2] = 1.0f;
        mat[3][3] = 1.0f;

        return mat;
    }

    inline mat4 rotation_x(float angle)
    {
        float s = sin(angle);
        float c = cos(angle);

        mat4 mat{};

        mat[0][0] = 1.0f;
        mat[1][1] = c;
        mat[1][2] = -s;
        mat[2][1] = s;
        mat[2][2] = c;
        mat[3][3] = 1.0f;

        return mat;
    }

    inline mat4 rotation_y(float angle)
    {
        float s = sin(angle);
        float c = cos(angle);

        mat4 mat;

        mat[0][0] = c;
        mat[0][2] = s;
        mat[1][1] = 1.0f;
        mat[2][0] = -s;
        mat[2][2] = c;
        mat[3][3] = 1.0f;

        return mat;
    }

    inline mat4 scale(vec3 s)
    {
        mat4 mat;

        mat[0][0] = s.x;
        mat[1][1] = s.y;
        mat[2][2] = s.z;
        mat[3][3] = 1.0f;

        return mat;
    }

    inline mat4 rotationXYZ(float angle, vec3 axis)
    {
        mat4 mat;
        float s = sinf(angle);
        float c = cosf(angle);
        float oc = 1.0f - c;

        mat[0][0] = oc * axis.x * axis.x + c;
        mat[0][1] = oc * axis.x * axis.y + axis.z * s;
        mat[0][2] = oc * axis.z * axis.x - axis.y * s;
        mat[0][3] = 0;

        mat[1][0] = oc * axis.x * axis.y - axis.z * s;
        mat[1][1] = oc * axis.y * axis.y + c;
        mat[1][2] = oc * axis.y * axis.z + axis.x * s;
        mat[1][3] = 0;

        mat[2][0] = oc * axis.z * axis.x + axis.y * s;
        mat[2][1] = oc * axis.y * axis.z - axis.x * s;
        mat[2][2] = oc * axis.z * axis.z + c;
        mat[2][3] = 0;

        mat[3][0] = 0;
        mat[3][1] = 0;
        mat[3][2] = 0;
        mat[3][3] = 1;

        return mat;
    }

    inline mat4 translation(vec3 t)
    {
        mat4 mat;
        mat[0][0] = 1.0f;
        mat[1][1] = 1.0f;
        mat[0][3] = t.x;
        mat[1][3] = t.y;
        mat[2][3] = t.z;
        mat[3][3] = 1.0f;
        return mat;
    }


    inline mat4 look_at(vec3 eye, vec3 target, vec3 up_dir)
    {
        mat4 mat;

        vec3 fwd = normalize(target - eye);
        auto side = normalize(cross(up_dir, fwd));
        auto up = normalize(cross(fwd, side));

        mat[0][0] = side.x;
        mat[0][1] = side.y;
        mat[0][2] = side.z;
        mat[0][3] = -dot(side, eye);

        mat[1][0] = up.x;
        mat[1][1] = up.y;
        mat[1][2] = up.z;
        mat[1][3] = -dot(up, eye);

        mat[2][0] = -fwd.x;
        mat[2][1] = -fwd.y;
        mat[2][2] = -fwd.z;
        mat[2][3] = dot(fwd, eye);

        mat[3][3] = 1;

        return mat;
    }


    inline mat4 perspective(float fov, float near, float far, float width, float height)
    {
        float h = tanf(fov * 0.5f);
        float w = h / (width / height);

        mat4 mat;

        mat[0][0] = w;
        mat[1][1] = h;
        mat[2][2] = (near + far) / (near - far);
        mat[2][3] = (2 * near * far) / (near - far);
        mat[3][2] = -1;
        mat[3][3] = 0;

        return mat;
    }

    inline mat4 ortho(float width, float height, float near, float far)
    {
        float right = width * 0.5f;
        float left = -right;

        float top = height * 0.5f;
        float bottom = -top;

        mat4 mat;

        mat[0][0] = 2 / (right - left);
        mat[0][3] = -(right + left) / (right - left);
        mat[1][1] = 2 / (top - bottom);
        mat[1][3] = -(top + bottom) / (top - bottom);
        mat[2][2] = -2 / (far - near);
        mat[2][3] = -(far + near) / (far - near);
        mat[3][3] = 1;

        return mat;
    }

    inline float radians(float degs)
    {
        return degs * float(M_PI) / 180.f;
    }

    inline mat4 transpose(const mat4& m)
    {
        mat4 res;

        res[0][0] = m[0][0];
        res[0][1] = m[1][0];
        res[0][2] = m[2][0];
        res[0][3] = m[3][0];

        res[1][0] = m[0][1];
        res[1][1] = m[1][1];
        res[1][2] = m[2][1];
        res[1][3] = m[3][1];

        res[2][0] = m[0][2];
        res[2][1] = m[1][2];
        res[2][2] = m[2][2];
        res[2][3] = m[3][2];

        res[3][0] = m[0][3];
        res[3][1] = m[1][3];
        res[3][2] = m[2][3];
        res[3][3] = m[3][3];

        return res;
    }


    inline mat4 inverse(const mat4& m)
    {
        auto tmp_0 = m[2][2] * m[3][3];
        auto tmp_1 = m[3][2] * m[2][3];
        auto tmp_2 = m[1][2] * m[3][3];
        auto tmp_3 = m[3][2] * m[1][3];
        auto tmp_4 = m[1][2] * m[2][3];
        auto tmp_5 = m[2][2] * m[1][3];
        auto tmp_6 = m[0][2] * m[3][3];
        auto tmp_7 = m[3][2] * m[0][3];
        auto tmp_8 = m[0][2] * m[2][3];
        auto tmp_9 = m[2][2] * m[0][3];
        auto tmp_10 = m[0][2] * m[1][3];
        auto tmp_11 = m[1][2] * m[0][3];
        auto tmp_12 = m[2][0] * m[3][1];
        auto tmp_13 = m[3][0] * m[2][1];
        auto tmp_14 = m[1][0] * m[3][1];
        auto tmp_15 = m[3][0] * m[1][1];
        auto tmp_16 = m[1][0] * m[2][1];
        auto tmp_17 = m[2][0] * m[1][1];
        auto tmp_18 = m[0][0] * m[3][1];
        auto tmp_19 = m[3][0] * m[0][1];
        auto tmp_20 = m[0][0] * m[2][1];
        auto tmp_21 = m[2][0] * m[0][1];
        auto tmp_22 = m[0][0] * m[1][1];
        auto tmp_23 = m[1][0] * m[0][1];

        auto t0 = (tmp_0 * m[1][1] + tmp_3 * m[2][1] + tmp_4 * m[3][1]) - (tmp_1 * m[1][1] + tmp_2 * m[2][1] + tmp_5 * m[3][1]);
        auto t1 = (tmp_1 * m[0][1] + tmp_6 * m[2][1] + tmp_9 * m[3][1]) - (tmp_0 * m[0][1] + tmp_7 * m[2][1] + tmp_8 * m[3][1]);
        auto t2 = (tmp_2 * m[0][1] + tmp_7 * m[1][1] + tmp_10 * m[3][1]) - (tmp_3 * m[0][1] + tmp_6 * m[1][1] + tmp_11 * m[3][1]);
        auto t3 = (tmp_5 * m[0][1] + tmp_8 * m[1][1] + tmp_11 * m[2][1]) - (tmp_4 * m[0][1] + tmp_9 * m[1][1] + tmp_10 * m[2][1]);

        mat4 dst;

        float d = 1.0f / (m[0][0] * t0 + m[1][0] * t1 + m[2][0] * t2 + m[3][0] * t3);

        dst[0][0] = d * t0;
        dst[0][1] = d * t1;
        dst[0][2] = d * t2;
        dst[0][3] = d * t3;
        dst[1][0] = d * ((tmp_1 * m[1][0] + tmp_2 * m[2][0] + tmp_5 * m[3][0]) - (tmp_0 * m[1][0] + tmp_3 * m[2][0] + tmp_4 * m[3][0]));
        dst[1][1] = d * ((tmp_0 * m[0][0] + tmp_7 * m[2][0] + tmp_8 * m[3][0]) - (tmp_1 * m[0][0] + tmp_6 * m[2][0] + tmp_9 * m[3][0]));
        dst[1][2] = d * ((tmp_3 * m[0][0] + tmp_6 * m[1][0] + tmp_11 * m[3][0]) - (tmp_2 * m[0][0] + tmp_7 * m[1][0] + tmp_10 * m[3][0]));
        dst[1][3] = d * ((tmp_4 * m[0][0] + tmp_9 * m[1][0] + tmp_10 * m[2][0]) - (tmp_5 * m[0][0] + tmp_8 * m[1][0] + tmp_11 * m[2][0]));
        dst[2][0] = d * ((tmp_12 * m[1][3] + tmp_15 * m[2][3] + tmp_16 * m[3][3]) - (tmp_13 * m[1][3] + tmp_14 * m[2][3] + tmp_17 * m[3][3]));
        dst[2][1] = d * ((tmp_13 * m[0][3] + tmp_18 * m[2][3] + tmp_21 * m[3][3]) - (tmp_12 * m[0][3] + tmp_19 * m[2][3] + tmp_20 * m[3][3]));
        dst[2][2] = d * ((tmp_14 * m[0][3] + tmp_19 * m[1][3] + tmp_22 * m[3][3]) - (tmp_15 * m[0][3] + tmp_18 * m[1][3] + tmp_23 * m[3][3]));
        dst[2][3] = d * ((tmp_17 * m[0][3] + tmp_20 * m[1][3] + tmp_23 * m[2][3]) - (tmp_16 * m[0][3] + tmp_21 * m[1][3] + tmp_22 * m[2][3]));
        dst[3][0] = d * ((tmp_14 * m[2][2] + tmp_17 * m[3][2] + tmp_13 * m[1][2]) - (tmp_16 * m[3][2] + tmp_12 * m[1][2] + tmp_15 * m[2][2]));
        dst[3][1] = d * ((tmp_20 * m[3][2] + tmp_12 * m[0][2] + tmp_19 * m[2][2]) - (tmp_18 * m[2][2] + tmp_21 * m[3][2] + tmp_13 * m[0][2]));
        dst[3][2] = d * ((tmp_18 * m[1][2] + tmp_23 * m[3][2] + tmp_15 * m[0][2]) - (tmp_22 * m[3][2] + tmp_14 * m[0][2] + tmp_19 * m[1][2]));
        dst[3][3] = d * ((tmp_22 * m[2][2] + tmp_16 * m[0][2] + tmp_21 * m[1][2]) - (tmp_20 * m[1][2] + tmp_23 * m[2][2] + tmp_17 * m[0][2]));
        return dst;
    }


    inline vec3 screen_coords_to_world_space_vector(
        vec2 pixel_position,
        vec2 screen_size,
        vec3 cam_eye,
        vec3 cam_target,
        vec3 cam_up,
        float cam_fov,
        float frustum_near,
        float frustum_far)
    {
        pixel_position = (pixel_position / screen_size) * 2.0f;
        pixel_position.x = pixel_position.x - 1.0f;
        pixel_position.y = 1.0f - pixel_position.y;
        pixel_position.y = pixel_position.y * tanf(cam_fov * 0.5f);
        pixel_position.x = pixel_position.x * tanf(cam_fov * 0.5f);
        auto inv_proj = inverse(perspective(cam_fov, frustum_near, frustum_far, screen_size.x, screen_size.y));
        auto inv_view = inverse(look_at(cam_eye, cam_target, cam_up));
        auto view_cords = vec4{pixel_position.x, pixel_position.y, -1, 1};
        view_cords = inv_proj * view_cords;
        view_cords.z = -1.0f;
        view_cords.w = 0.0f;
        auto world_cords = inv_view * view_cords;

        return normalize(vec3{world_cords.x, world_cords.y, world_cords.z});
    }

    template<typename DataType, size_t Rows, size_t Cols>
    const auto* value_ptr(const mat<Rows, Cols, DataType>& m)
    {
        return &m[0][0];
    }

    template<typename DataType, size_t Rows, size_t Cols>
    auto* value_ptr(mat<Rows, Cols, DataType>& m)
    {
        return &m[0][0];
    }
} // namespace math
