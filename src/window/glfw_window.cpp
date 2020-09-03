

#include "glfw_window.hpp"


#include <glad/glad.h>
#include <GLFW/glfw3.h>


renderer::glfw_window::glfw_window(const std::string& name, misc::size size, std::function<void(float)> updater)
: window(size)
, m_updater(std::move(updater))
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_size.width, m_size.height, name.c_str(), nullptr, nullptr);

    if (m_window == nullptr)
    {
        destroy_window();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        destroy_window();
        throw std::runtime_error("Failed to initialize GLAD");
    }
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

    m_updater(glfwGetTime());
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
