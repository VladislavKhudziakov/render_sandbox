


#pragma once

#include <renderer/renderer.hpp>
#include <misc/misc.hpp>

#include <glad/glad.h>

namespace renderer::gl::traits
{
    struct gl_type
    {
        GLuint gl_format;
        size_t type_size;
    };


    inline gl_type get_gl_type(::renderer::type type)
    {
        switch (type) {
            case type::f32:
                return {GL_FLOAT, sizeof(float)};
            case type::f16:
                return {GL_FLOAT, sizeof(float) / 2};
            case type::u32:
                return {GL_UNSIGNED_INT, sizeof(uint32_t)};
            case type::u16:
                return {GL_UNSIGNED_SHORT, sizeof(uint16_t)};
            case type::u8:
                return {GL_UNSIGNED_BYTE, sizeof(uint8_t)};
            default:
                ASSERT(false && "invalid type.");
        }
    }


    inline GLenum get_gl_shader_stage(::renderer::shader_stage_name shader_stage_name)
    {
        switch (shader_stage_name) {
            case shader_stage_name::vertex:
                return GL_VERTEX_SHADER;
            case shader_stage_name::fragment:
                return GL_FRAGMENT_SHADER;
        }
    }


    inline GLenum get_gl_texture_type(const ::renderer::texture_descriptor& descriptor)
    {
        switch (descriptor.type) {
            case texture_type::d2:
                return descriptor.size.length == 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY;
            case texture_type::d3:
                return  GL_TEXTURE_3D;
            case texture_type::cube:
                return descriptor.size.length == 1 ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_CUBE_MAP_ARRAY;
            default:
                ASSERT(false && "invalid texture type.");
        }
    }


    inline std::tuple<GLenum, GLenum, GLenum> get_texture_formats(const ::renderer::texture_descriptor& descriptor)
    {
        switch (descriptor.data_type) {
            case type::f32:
                switch (descriptor.format) {
                    case texture_format::r:
                        return {GL_FLOAT, GL_R32F, GL_R};
                    case texture_format::rg:
                        return {GL_FLOAT, GL_RG32F, GL_RG};
                    case texture_format::rgba:
                        return {GL_FLOAT, GL_RGBA32F, GL_RGBA};
                }
            case type::f16:
                switch (descriptor.format) {
                    case texture_format::r:
                        return {GL_FLOAT, GL_R16F, GL_R};
                    case texture_format::rg:
                        return {GL_FLOAT, GL_RG16F, GL_RG};
                    case texture_format::rgba:
                        return {GL_FLOAT, GL_RGBA16F, GL_RGBA};
                }
            case type::u32:
                switch (descriptor.format) {
                    case texture_format::r:
                        return {GL_UNSIGNED_INT, GL_R32UI, GL_R};
                    case texture_format::rg:
                        return {GL_UNSIGNED_INT, GL_RG32UI, GL_RG};
                    case texture_format::rgba:
                        return {GL_UNSIGNED_INT, GL_RGBA32UI, GL_RGBA};
                }
            case type::u16:
                switch (descriptor.format) {
                    case texture_format::r:
                        return {GL_UNSIGNED_SHORT, GL_R16, GL_R};
                    case texture_format::rg:
                        return {GL_UNSIGNED_SHORT, GL_RG16, GL_RG};
                    case texture_format::rgba:
                        return {GL_UNSIGNED_SHORT, GL_RGBA16, GL_RGBA};
                }
            case type::u8:
                switch (descriptor.format) {
                    case texture_format::r:
                        return {GL_UNSIGNED_BYTE, GL_R8, GL_R};
                    case texture_format::rg:
                        return {GL_UNSIGNED_BYTE, GL_RG8, GL_RG};
                    case texture_format::rgba:
                        return {GL_UNSIGNED_BYTE, GL_RGBA8, GL_RGBA};
                }
            default:
                ASSERT(false && "invalid texture data type.");
        }
    }


    inline size_t get_parameter_size(::renderer::parameter_type type)
    {
        switch(type) {
            case parameter_type::vec4:
                return sizeof(float) * 4;
            case parameter_type::mat2x4:
                return 2 * (sizeof(float) * 4);
            case parameter_type::mat3x4:
                return 3 * (sizeof(float) * 4);
            case parameter_type::mat4:
                return 4 * (sizeof(float) * 4);
        }
    }

} // namespace renderer::gl::traits
