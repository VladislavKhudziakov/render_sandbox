

#include "texture.hpp"

#include <renderer/gl/traits.hpp>
#include <renderer/gl/bind_guard.hpp>
#include <misc/misc.hpp>


renderer::gl::texture::texture(const ::renderer::texture_descriptor& desc)
    : m_type(traits::get_gl_texture_type(desc))
{
    if (desc.pixels_data.empty()) {
        return;
    }

    bind_guard bind(*this);

    switch (desc.type) {
        case texture_type::d2:
            load_2d_data(desc);
            break;
        case texture_type::d3:
            load_3d_data(desc);
            break;
        case texture_type::cube:
            load_cube_data(desc);
            break;
    }

    if (desc.mips) {
        glGenerateMipmap(m_type);
    }

    switch (desc.filtration) {
        case texture_filtration::point:
            glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case texture_filtration::bilinear:
            glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case texture_filtration::trilinear:
            ASSERT(desc.mips);
            glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
    }
}


void renderer::gl::texture::bind()
{
    glBindTexture(m_type, m_handler);
}


void renderer::gl::texture::unbind()
{
    glBindTexture(m_type, 0);
}


void renderer::gl::texture::load_2d_data(const ::renderer::texture_descriptor& descriptor)
{
    const auto [type, int_fmt, fmt] = traits::get_texture_formats(descriptor);

    ASSERT(descriptor.size.length > 0);

    if (descriptor.size.length == 1) {
        glTexImage2D(
            m_type,
            0,
            int_fmt,
            descriptor.size.width,
            descriptor.size.height,
            0,
            fmt,
            type,
            descriptor.pixels_data.data());
    } else {
        glTexImage3D(
            m_type,
            0,
            int_fmt,
            descriptor.size.width,
            descriptor.size.height,
            descriptor.size.length,
            0,
            fmt,
            type,
            descriptor.pixels_data.data());
    }
}


void renderer::gl::texture::load_cube_data(const ::renderer::texture_descriptor& descriptor)
{
    size_t elements_per_pixel = 0;

    switch (descriptor.format) {
        case texture_format::r:
            elements_per_pixel = 1;
            break;
        case texture_format::rg:
            elements_per_pixel = 2;
            break;
        case texture_format::rgba:
            elements_per_pixel = 4;
            break;
    }

    const auto [type, type_size] = traits::get_gl_type(descriptor.data_type);
    const auto img_size = descriptor.size.width * descriptor.size.height * elements_per_pixel * type_size;
    const auto [texture_type, int_fmt, fmt] = traits::get_texture_formats(descriptor);

    auto data_ptr = descriptor.pixels_data.data();

    for (size_t i = 0; i < 6; ++i, data_ptr += img_size) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            int_fmt,
            descriptor.size.width,
            descriptor.size.height,
            0,
            fmt,
            texture_type,
            data_ptr);
    }
}


void renderer::gl::texture::load_3d_data(const ::renderer::texture_descriptor& descriptor)
{
    const auto [type, int_fmt, fmt] = traits::get_texture_formats(descriptor);
    glTexImage3D(
        m_type,
        0,
        int_fmt,
        descriptor.size.width,
        descriptor.size.height,
        descriptor.size.depth,
        0,
        fmt,
        type,
        descriptor.pixels_data.data());
}
