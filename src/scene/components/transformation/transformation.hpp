

#pragma once

#include <math/matrix.hpp>
#include <math/matrix_operations.hpp>
#include <math/quaternion.hpp>

#include <scene/components/component_base.hpp>

namespace renderer::scene
{
    class transformation : public component_base
    {
    public:
        transformation() = default;
        ~transformation() override = default;
        transformation(const math::mat4& transform, const math::mat4& inv_transform);
        transformation& operator*(const transformation& r);
        void col_maj();

        math::mat4 transform;
        math::mat4 inverse_transform;
    };

    transformation translation(float x, float y, float z);
    transformation scale(float x, float y, float z);
    transformation rotation(math::quaternion q);
} // namespace renderer::scene
