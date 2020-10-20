

#pragma once

#include <math/matrix.hpp>

namespace renderer
{
    class camera
    {
    public:
        void update();
        const math::mat4& get_transformation();
        const math::mat4& get_view() const;
        const math::mat4& get_proj() const;

        math::vec3 position{0, 0, 1};
        math::vec3 target_position{0, 0, 0};
        float fov{M_PI / 2};
        float near{1};
        float far{100};
        float width{1};
        float height{1};

    private:
        math::mat4 m_transformation;
        math::mat4 m_view;
        math::mat4 m_proj;
    };
} // namespace renderer
