

#include "shader.hpp"

#include <renderer/renderer.hpp>
#include <renderer/gl/traits.hpp>

#include <iostream>


renderer::gl::shader::shader(const ::renderer::shader_descriptor& descriptor)
{
    std::vector<detail::shader_raii_wrap> shaders;
    shaders.reserve(descriptor.stages.size());

    for (const auto& stage : descriptor.stages) {
        shaders.emplace_back(compile_shader(stage));
        glAttachShader(m_handler, shaders.back());
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
}


void renderer::gl::shader::bind()
{
    glUseProgram(m_handler);
}


void renderer::gl::shader::unbind()
{
    glUseProgram(0);
}


renderer::gl::detail::shader_raii_wrap renderer::gl::shader::compile_shader(const ::renderer::shader_stage& stage)
{
    detail::shader_raii_wrap handler(glCreateShader(traits::get_gl_shader_stage(stage.name)));
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
