

#include "renderer.hpp"

renderer::mesh_handler renderer::gl::renderer::create_mesh(
    const ::renderer::mesh_layout_descriptor& descriptor)
{
    m_vaos.emplace_back(descriptor);
    return m_vaos.size() - 1;
}


renderer::shader_handler renderer::gl::renderer::create_shader(
    const ::renderer::shader_descriptor& descriptor)
{
    m_shaders.emplace_back(descriptor);
    return m_shaders.size() - 1;
}


void renderer::gl::renderer::draw(
    ::renderer::mesh_handler mesh_handler,
    ::renderer::shader_handler shader_handler,
    uint32_t instances_count)
{
    constexpr static ::renderer::shader_state default_state{};

    auto& shader = m_shaders[shader_handler];

    bind_guard shader_bind(shader);
    auto& shader_samplers = shader.m_samplers;

    uint32_t sampler_index = 0;

    for (const auto& [sampler_name, texture_index] : shader_samplers) {
        glActiveTexture(GL_TEXTURE0 + sampler_index++);
        m_textures[texture_index].bind();
    }

    set_gpu_state(shader.m_state);

    if (instances_count == 1) {
        m_vaos[mesh_handler].draw();
    } else {
        m_vaos[mesh_handler].draw_instanced(instances_count);
    }

    set_gpu_state(default_state);
}


void renderer::gl::renderer::update(float time)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLint  main_fb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &main_fb);

    glClearColor(0, 0, 0, 1);
    glClearDepth(-1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& params_list : m_params_lists) {
        params_list.load_data_to_gpu();
    }

    int32_t last_pass = -1;

    for (const auto& command : m_commands_buffer) {
        switch (command.type) {
            case draw_command_type::pass:
                if (last_pass >= 0) {
                    m_passes[last_pass].end();
                }
                m_passes[command.pass].begin();
                last_pass = command.pass;
                break;
            case draw_command_type::draw:
                draw(command.mesh, command.shader, command.instances_count);
                break;
        }
    }

    if (last_pass >= 0) {
        auto& src_pass = m_passes[last_pass];
        m_passes[last_pass].end();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, main_fb);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_passes[last_pass].m_framebuffer_handler);
        glBlitFramebuffer(0, 0, src_pass.m_width, src_pass.m_height, viewport[0], viewport[1], viewport[2], viewport[3], GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    m_commands_buffer.clear();
}


renderer::texture_handler renderer::gl::renderer::create_texture(
    const ::renderer::texture_descriptor& descriptor)
{
    m_textures.emplace_back(descriptor);
    return m_textures.size() - 1;
}


void renderer::gl::renderer::set_shader_sampler(
    ::renderer::shader_handler shader_handler,
    ::renderer::texture_handler texture_handler,
    const std::string& sampler_name)
{
    auto& shader = m_shaders[shader_handler];
    shader.m_samplers[sampler_name] = texture_handler;
}


::renderer::parameters_list_handler renderer::gl::renderer::create_parameters_list(const ::renderer::parameters_list_descriptor& descriptor)
{
    auto& params_list = m_params_lists.emplace_back(descriptor);

    const auto binding_index = m_params_lists.size() - 1;
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_index, params_list.m_gpu_storage->m_handler);

    return binding_index;
}


void renderer::gl::renderer::set_parameter_data(::renderer::parameters_list_handler handler, uint32_t param_index, void* data)
{
    m_params_lists[handler].set_parameter_data(param_index, data);
}


void renderer::gl::renderer::destroy_mesh(::renderer::mesh_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    m_vaos.erase(m_vaos.begin() + handler);
}


void renderer::gl::renderer::destroy_shader(::renderer::shader_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    m_shaders.erase(m_shaders.begin() + handler);
}


void renderer::gl::renderer::destroy_texture(::renderer::texture_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    m_textures.erase(m_textures.begin() + handler);
}


void renderer::gl::renderer::destroy_parameters_list(::renderer::parameters_list_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    m_params_lists.erase(m_params_lists.begin() + handler);
}


void renderer::gl::renderer::encode_draw_command(::renderer::draw_command command)
{
    m_commands_buffer.emplace_back(command);
}


void renderer::gl::renderer::set_gpu_state(const ::renderer::shader_state& state)
{
    switch (state.depth_test) {
        case depth_test_mode::less:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            break;
        case depth_test_mode::less_eq:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            break;
        case depth_test_mode::greater:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_GREATER);
            break;
        case depth_test_mode::greater_eq:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_GEQUAL);
            break;
        case depth_test_mode::off:
            glDisable(GL_DEPTH_TEST);
            break;
    }

    glDepthMask(state.depth_write);
    glColorMask(state.color_write, state.color_write, state.color_write, state.color_write);

    switch (state.cull) {
        case cull_mode::off:
            glDisable(GL_CULL_FACE);
            break;
        case cull_mode::back:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;
        case cull_mode::front:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;
    }

    switch (state.blend) {
        case blend_mode::off:
            glDisable(GL_BLEND);
            break;
        case blend_mode::alpha:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case blend_mode::add:
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case blend_mode::multiply:
            glEnable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
    }
}


renderer::pass_handler renderer::gl::renderer::create_pass(const ::renderer::pass_descriptor& descriptor)
{
    m_passes.emplace_back(descriptor, m_textures);
    return m_passes.size() - 1;
}


void renderer::gl::renderer::destroy_pass(::renderer::pass_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    auto it = m_passes.begin() + handler;
    m_passes.erase(it);
}


void renderer::gl::renderer::resize_pass(::renderer::pass_handler handler, size_t w, size_t h)
{
    m_passes[handler].resize(w, h, m_textures);
}
