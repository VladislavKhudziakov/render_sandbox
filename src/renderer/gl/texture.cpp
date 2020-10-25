

#include "texture.hpp"

#include <renderer/gl/traits.hpp>
#include <renderer/gl/bind_guard.hpp>
#include <misc/misc.hpp>


renderer::gl::texture::texture(const ::renderer::texture_descriptor& desc)
    : m_gl_type(traits::get_gl_texture_type(desc))
    , m_type(desc.type)
    , m_storage_data(traits::get_texture_formats(desc))
{
    bind_guard bind(*this);

    switch (m_type) {
        case texture_type::d2:
            load_2d_data(desc);
            break;
        case texture_type::d3:
            load_3d_data(desc);
            break;
        case texture_type::cube:
            load_cube_data(desc);
            break;
        case texture_type::attachment:
            resize(desc.size);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            return;
    }

    if (desc.mips) {
        ASSERT(m_type != texture_type::attachment);
        glGenerateMipmap(m_gl_type);
    }

    switch (desc.filtration) {
        case texture_filtration::point:
            glTexParameteri(m_gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(m_gl_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case texture_filtration::bilinear:
            glTexParameteri(m_gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(m_gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case texture_filtration::trilinear:
            ASSERT(desc.mips);
            glTexParameteri(m_gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(m_gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
    }
}


void renderer::gl::texture::bind()
{
    glBindTexture(m_gl_type, m_handler);
}


void renderer::gl::texture::unbind()
{
    glBindTexture(m_gl_type, 0);
}


void renderer::gl::texture::load_2d_data(const ::renderer::texture_descriptor& descriptor)
{
    const auto [type, int_fmt, fmt] = m_storage_data;

    ASSERT(descriptor.size.length > 0);

    auto* data_ptr = descriptor.pixels.empty() ? nullptr : descriptor.pixels.data();

    if (descriptor.size.length == 1) {
        glTexImage2D(m_gl_type, 0, int_fmt, descriptor.size.width, descriptor.size.height, 0, fmt, type, data_ptr);
    } else {
        glTexImage3D(m_gl_type, 0, int_fmt, descriptor.size.width, descriptor.size.height, descriptor.size.length, 0, fmt, type, data_ptr);
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

    const auto [type, type_size] = traits::get_gl_type(descriptor.pixels_data_type);
    const auto img_size = descriptor.size.width * descriptor.size.height * elements_per_pixel * type_size;
    const auto [texture_type, int_fmt, fmt] = m_storage_data;

    auto* data_ptr = descriptor.pixels.empty() ? nullptr : descriptor.pixels.data();

    for (size_t i = 0; i < 6; ++i, data_ptr += img_size) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, int_fmt, descriptor.size.width, descriptor.size.height, 0, fmt, texture_type, data_ptr);
    }
}


void renderer::gl::texture::load_3d_data(const ::renderer::texture_descriptor& descriptor)
{
    auto* data_ptr = descriptor.pixels.empty() ? nullptr : descriptor.pixels.data();
    const auto [type, int_fmt, fmt] = m_storage_data;
    glTexImage3D(m_gl_type, 0, int_fmt, descriptor.size.width, descriptor.size.height, descriptor.size.depth, 0, fmt, type, data_ptr);
}


void renderer::gl::texture::resize(const ::renderer::texture_size& size)
{
    bind_guard bind(*this);

    const auto [type, int_fmt, fmt] = m_storage_data;
    ASSERT(m_type == ::renderer::texture_type::attachment);
    glTexImage2D(m_gl_type, 0, int_fmt, size.width, size.height, 0, fmt, type, nullptr);

    glTexParameteri(m_gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(m_gl_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}


void renderer::gl::texture::load(::renderer::texture_size size, void* data_ptr)
{
    const auto [type, int_fmt, fmt] = m_storage_data;
    glTexImage2D(m_gl_type, 0, int_fmt, size.width, size.height, 0, fmt, type, data_ptr);
}
