

#include "shader.hpp"

#include <renderer/renderer.hpp>
#include <renderer/gl/traits.hpp>
#include <misc/misc.hpp>

#include <iostream>


renderer::gl::shader::shader(const ::renderer::shader_descriptor& descriptor)
    : m_samplers(descriptor.samplers)
{
    for (const auto& stage : descriptor.stages) {
        glAttachShader(m_handler, compile_shader(stage));
    }

    glLinkProgram(m_handler);

    GLint link_status = 0;
    GLint log_len = 0;
    glGetProgramiv(m_handler, GL_LINK_STATUS, &link_status);
    glGetProgramiv(m_handler, GL_INFO_LOG_LENGTH, &log_len);

    if (log_len > 0) {
        std::string log_buffer;
        log_buffer.resize(log_len);

        glGetProgramInfoLog(m_handler, log_len, nullptr, log_buffer.data());

        if (link_status == GL_FALSE) {
            throw std::runtime_error(log_buffer);
        }

        std::cout << log_buffer << std::endl;
    }

    uint32_t texture_slot = 0;

    for (const auto& [sampler_name, sampler_tex] : m_samplers) {
        auto loc = glGetUniformLocation(m_handler, sampler_name.c_str());
        ASSERT(loc >= 0);
        glUniform1i(loc, texture_slot++);
    }
}


void renderer::gl::shader::bind()
{
    glUseProgram(m_handler);
}


void renderer::gl::shader::unbind()
{
    glUseProgram(0);
}


renderer::gl::detail::stage_handler renderer::gl::shader::compile_shader(const ::renderer::shader_stage& stage)
{
    detail::stage_handler handler(traits::get_gl_shader_stage(stage.name));
    const char* c_str = stage.code.c_str();
    glShaderSource(handler, 1, &c_str, nullptr);
    glCompileShader(handler);

    GLint compile_status = 0;
    GLint log_len = 0;

    glGetShaderiv(handler, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(handler, GL_INFO_LOG_LENGTH, &log_len);

    if (log_len > 0) {
        std::string log_buffer;
        log_buffer.resize(log_len);

        glGetShaderInfoLog(handler, log_len, nullptr, log_buffer.data());

        if (compile_status == GL_FALSE) {
            throw std::runtime_error(log_buffer);
        }

        std::cout << log_buffer << std::endl;
    }

    return handler;
}
