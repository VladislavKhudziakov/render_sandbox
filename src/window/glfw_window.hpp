

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

        void register_mouse_position_handler(mouse_pos_event_handler function) override;
        void register_mouse_click_handler(mouse_click_event_handler handler) override;
        void register_resize_handler(resize_handler handler) override;
        void register_mouse_scroll_callback(scroll_handler handler) override;
        void register_key_handler(keyboard_handler handler) override;

        misc::size get_view_size() override;

        void get_pixel_color(void* data, size_t x, size_t y, texture_format fmt, data_type type);

    private:
        static void mouse_pos_callback(GLFWwindow* window, double x, double y);
        static void mouse_button_callback(GLFWwindow*, int button, int action, int mods);
        static void resize_callback(GLFWwindow* window, int width, int height);
        static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
        static void key_callback(GLFWwindow*,int,int,int,int);

        void destroy_window();

        std::vector<mouse_pos_event_handler> m_mouse_pos_handlers;
        std::vector<mouse_click_event_handler> m_mouse_click_handlers;
        std::vector<resize_handler> m_resize_handlers;
        std::vector<scroll_handler> m_scroll_handlers;
        std::vector<keyboard_handler> m_keyboard_handlers;

        GLFWwindow* m_window;
    };
} // namespace renderer
