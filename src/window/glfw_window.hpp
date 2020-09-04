

#pragma once

#include <window/window.hpp>
#include <string>
#include <functional>

class GLFWwindow;

namespace renderer
{
    class glfw_window : public window
    {
    public:
        glfw_window(const std::string& name, misc::size);

        glfw_window(const glfw_window&) = delete;
        glfw_window(glfw_window&&) = delete;
        glfw_window& operator=(const glfw_window&) = delete;
        glfw_window& operator=(glfw_window&&) = delete;

        ~glfw_window() override;
        void update() override;
        void close() override;
        bool closed() override;

    private:
        void destroy_window();
        GLFWwindow* m_window;
    };
} // namespace renderer
