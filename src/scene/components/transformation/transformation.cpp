

#include "transformation.hpp"


renderer::scene::transformation& renderer::scene::transformation::operator*(const renderer::scene::transformation& r)
{
    transform = transform * r.transform;
    inverse_transform = inverse_transform * r.inverse_transform;
    return *this;
}


renderer::scene::transformation::transformation(const math::mat4& transform, const math::mat4& inv_transform)
    : transform(transform)
    , inverse_transform(inv_transform)
{
}


void renderer::scene::transformation::col_maj()
{
    transform = math::transpose(transform);
    inverse_transform = math::transpose(inverse_transform);
}


renderer::scene::transformation renderer::scene::scale(float x, float y, float z)
{
    transformation t{};


    t.transform[0][0] = x;
    t.transform[1][1] = y;
    t.transform[2][2] = z;
    t.transform[3][3] = 1.0f;

    t.inverse_transform[0][0] = 1.f / x;
    t.inverse_transform[1][1] = 1.f / y;
    t.inverse_transform[2][2] = 1.f / z;
    t.inverse_transform[3][3] = 1.0f;

    return t;
}


renderer::scene::transformation renderer::scene::translation(float x, float y, float z)
{
    transformation t;

    t.transform[0][0] = 1.0f;
    t.transform[1][1] = 1.0f;
    t.transform[0][3] = x;
    t.transform[1][3] = y;
    t.transform[2][3] = z;
    t.transform[3][3] = 1.0f;

    t.inverse_transform[0][0] = 1.0f;
    t.inverse_transform[1][1] = 1.0f;
    t.inverse_transform[0][3] = -x;
    t.inverse_transform[1][3] = -y;
    t.inverse_transform[2][3] = -z;
    t.inverse_transform[3][3] = 1.0f;

    return t;
}


renderer::scene::transformation renderer::scene::rotation(math::quaternion q)
{
    transformation t;
    t.transform = math::to_matrix(q);
    t.inverse_transform = math::inverse(t.transform);
    return t;
}
