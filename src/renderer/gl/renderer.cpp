

#include "renderer.hpp"

#include <misc/opengl.hpp>

renderer::mesh_handler renderer::gl::renderer::create_mesh(
    const ::renderer::mesh_layout_descriptor& descriptor)
{
    return m_factory.create<vao>(descriptor);
}


renderer::shader_handler renderer::gl::renderer::create_shader(
    const ::renderer::shader_descriptor& descriptor)
{
    return m_factory.create<shader>(descriptor);
}


void renderer::gl::renderer::draw(
    ::renderer::mesh_handler mesh_handler,
    ::renderer::shader_handler shader_handler,
    uint32_t instances_count,
    uint32_t draw_id)
{
    constexpr static ::renderer::shader_state default_state{};

    auto shader_view = m_factory.view<shader>();
    auto& shader = shader_view[shader_handler];

    auto& shader_samplers = shader.m_samplers;

    bind_guard shader_bind(shader);

    uint32_t sampler_index = 0;

    auto textures_view = m_factory.view<texture>();

    for (const auto& [sampler_name, texture_index] : shader_samplers) {
        glActiveTexture(GL_TEXTURE0 + sampler_index++);
        textures_view[texture_index].bind();
    }

    auto loc = glGetUniformLocation(shader.m_handler, "DrawID");
    if (loc >= 0) {
        glUniform1i(loc, draw_id);
    }

    set_gpu_state(shader.m_state);

    auto mesh_view = m_factory.view<vao>();

    if (instances_count == 1) {
        mesh_view[mesh_handler].draw();
    } else {
        mesh_view[mesh_handler].draw_instanced(instances_count);
    }

    set_gpu_state(default_state);
}


void renderer::gl::renderer::update(float time)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLint main_fb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &main_fb);

    glClearColor(0, 0, 0, 1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto params_list_view = m_factory.view<parameters_list>();
    auto params_lists_impl = params_list_view.get_pool()->objects_view();

    for (auto& params_list : params_lists_impl) {
        params_list->load_data_to_gpu();
    }

    int32_t last_pass = -1;

    auto passes_view = m_factory.view<render_pass>();

    for (const auto& command : m_commands_buffer) {
        switch (command.type) {
            case draw_command_type::pass:
                if (last_pass >= 0) {
                    passes_view[last_pass].end();
                }
                passes_view[command.pass].begin();
                last_pass = command.pass;
                break;
            case draw_command_type::draw:
                draw(command.mesh, command.shader, command.instances_count, command.draw_id);
                break;
        }
    }

    if (last_pass >= 0) {
        auto& src_pass = passes_view[last_pass];
        passes_view[last_pass].end();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, main_fb);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, passes_view[last_pass].m_framebuffer_handler);
        glBlitFramebuffer(0, 0, src_pass.m_width, src_pass.m_height, viewport[0], viewport[1], viewport[2], viewport[3], GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    m_commands_buffer.clear();
}


renderer::texture_handler renderer::gl::renderer::create_texture(
    const ::renderer::texture_descriptor& descriptor)
{
    return m_factory.create<texture>(descriptor);
}


void renderer::gl::renderer::set_shader_sampler(
    ::renderer::shader_handler shader_handler,
    ::renderer::texture_handler texture_handler,
    const std::string& sampler_name)
{
    auto shader_view = m_factory.view<shader>();

    auto& shader = shader_view[shader_handler];
    shader.m_samplers[sampler_name] = texture_handler;
}


::renderer::parameters_list_handler renderer::gl::renderer::create_parameters_list(const ::renderer::parameters_list_descriptor& descriptor)
{
    auto binding_index = m_factory.create<parameters_list>(descriptor);
    auto& params_list = m_factory.view<parameters_list>()[binding_index];
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_index, params_list.m_gpu_storage->m_handler);

    return binding_index;
}


void renderer::gl::renderer::set_parameter_data(::renderer::parameters_list_handler handler, uint32_t param_index, void* data)
{
    m_factory.view<parameters_list>()[handler].set_parameter_data(param_index, data);
}


void renderer::gl::renderer::destroy_mesh(::renderer::mesh_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    m_factory.destroy<vao>(handler);
}


void renderer::gl::renderer::destroy_shader(::renderer::shader_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    m_factory.destroy<shader>(handler);
}


void renderer::gl::renderer::destroy_texture(::renderer::texture_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    return m_factory.destroy<texture>(handler);
}


void renderer::gl::renderer::destroy_parameters_list(::renderer::parameters_list_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    m_factory.destroy<parameters_list>(handler);
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
    auto textures_view = m_factory.view<texture>();
    return m_factory.create<render_pass>(descriptor, textures_view);
}


void renderer::gl::renderer::destroy_pass(::renderer::pass_handler handler)
{
    if (handler == ::renderer::null) {
        return;
    }

    m_factory.destroy<render_pass>(handler);
}


void renderer::gl::renderer::resize_pass(::renderer::pass_handler handler, size_t w, size_t h)
{
    auto textures_view = m_factory.view<texture>();
    m_factory.view<render_pass>()[handler].resize(w, h, textures_view);
}


void renderer::gl::renderer::load_texture_data(
    ::renderer::texture_handler handler,
    ::renderer::texture_size size,
    void* data)
{
    m_factory.view<texture>()[handler].load(size, data);
}
