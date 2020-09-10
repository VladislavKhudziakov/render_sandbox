

#include "glfw_window.hpp"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <renderer/gl/renderer.hpp>
#include <renderer/gl/traits.hpp>


renderer::glfw_window::glfw_window(const std::string& name, misc::size size)
    : window(size, std::make_unique<gl::renderer>())
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_size.width, m_size.height, name.c_str(), nullptr, nullptr);

    if (m_window == nullptr) {
        destroy_window();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        destroy_window();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetWindowUserPointer(m_window, this);

    glfwSetWindowSizeCallback(m_window, &resize_callback);
    glfwSetCursorPosCallback(m_window, &mouse_pos_callback);
    glfwSetMouseButtonCallback(m_window, &mouse_button_callback);
    glfwSetScrollCallback(m_window, &scroll_callback);
}


renderer::glfw_window::~glfw_window()
{
    destroy_window();
}


void renderer::glfw_window::update()
{
    int32_t w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    glViewport(0, 0, w, h);
    m_renderer->update(glfwGetTime());
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}


void renderer::glfw_window::destroy_window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}


void renderer::glfw_window::close()
{
    destroy_window();
}


bool renderer::glfw_window::closed()
{
    return glfwWindowShouldClose(m_window);
}


void renderer::glfw_window::mouse_pos_callback(GLFWwindow* window, double x, double y)
{
    auto glfw_window = reinterpret_cast<::renderer::glfw_window*>(glfwGetWindowUserPointer(window));

    if (x < 0 || x > glfw_window->m_size.width) {
        return;
    }

    if (y < 0 || y > glfw_window->m_size.height) {
        return;
    }


    for (auto& handler : glfw_window->m_mouse_pos_handlers) {
        handler(::renderer::mouse_position_event{x, y});
    }
}


void renderer::glfw_window::resize_callback(GLFWwindow* window, int width, int height)
{
    auto glfw_window = reinterpret_cast<::renderer::glfw_window*>(glfwGetWindowUserPointer(window));
    glfw_window->m_size = {uint32_t(width), uint32_t(height)};

    for (auto& handler : glfw_window->m_resize_handlers) {
        handler({width, height});
    }
}


void renderer::glfw_window::register_mouse_position_handler(::renderer::mouse_pos_event_handler handler)
{
    m_mouse_pos_handlers.emplace_back(std::move(handler));
}


void renderer::glfw_window::register_mouse_click_handler(::renderer::mouse_click_event_handler handler)
{
    m_mouse_click_handlers.emplace_back(std::move(handler));
}


void renderer::glfw_window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    mouse_click_event e {};

    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            e.button = mouse_click_event::button_type::left;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            e.button = mouse_click_event::button_type::right;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            e.button = mouse_click_event::button_type::middle;
            break;
        default:
            return;
    }

    switch (action) {
        case GLFW_PRESS:
            e.action = mouse_click_event::action_type::press;
            break;
        case GLFW_RELEASE:
            e.action = mouse_click_event::action_type::release;
            break;
        default:
            return;
    }

    auto glfw_window = reinterpret_cast<::renderer::glfw_window*>(glfwGetWindowUserPointer(window));

    for (auto& handler : glfw_window->m_mouse_click_handlers) {
        handler(e);
    }
}


void renderer::glfw_window::register_resize_handler(::renderer::resize_handler handler)
{
    m_resize_handlers.emplace_back(std::move(handler));
}


void renderer::glfw_window::get_pixel_color(void* data, size_t x, size_t y, texture_format fmt, data_type type)
{
    glReadPixels(x, y, 1, 1, gl::traits::get_gl_fmt(fmt), gl::traits::get_gl_type(type).gl_format, data);
}


void renderer::glfw_window::register_mouse_scroll_callback(::renderer::scroll_handler handler)
{
    m_scroll_handlers.emplace_back(std::move(handler));
}


void renderer::glfw_window::scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    auto glfw_window = reinterpret_cast<::renderer::glfw_window*>(glfwGetWindowUserPointer(window));

    for(auto& handler : glfw_window->m_scroll_handlers) {
        handler({x_offset, y_offset});
    }
}


misc::size renderer::glfw_window::get_view_size()
{
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    return {uint32_t(w), uint32_t(h)};
}
