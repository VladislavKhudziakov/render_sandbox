


#pragma once

#include <renderer/renderer.hpp>

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
                assert(false && "invalid type.");
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

} // namespace renderer::gl::traits
