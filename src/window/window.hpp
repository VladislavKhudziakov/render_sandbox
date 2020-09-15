


#pragma once

#include <misc/misc.hpp>

#include <renderer/renderer.hpp>

namespace renderer
{
    struct mouse_position_event
    {
        double x; double y;
    };

    enum class action_type
    {
        press, release, hold
    };

    struct mouse_click_event
    {
        enum class button_type
        {
            left, right, middle
        };

        button_type button;
        action_type action;
    };

    struct resize_event
    {
        int width, height;
    };

    struct scroll_event
    {
        double x_offset, y_offset;
    };

    struct key_event
    {
        int key_code;
        action_type action;
    };

    using mouse_pos_event_handler = std::function<void(mouse_position_event)>;
    using mouse_click_event_handler = std::function<void(mouse_click_event)>;
    using resize_handler = std::function<void(resize_event)>;
    using scroll_handler = std::function<void(scroll_event)>;
    using keyboard_handler = std::function<void(key_event)>;


    class window
    {
    public:
        explicit window(misc::size, std::unique_ptr<::renderer::renderer>);
        virtual ~window() = default;

        virtual void update() = 0;

        virtual void close() = 0;
        virtual bool closed() = 0;

        virtual void register_mouse_position_handler(mouse_pos_event_handler) = 0;
        virtual void register_mouse_click_handler(mouse_click_event_handler) = 0;
        virtual void register_mouse_scroll_callback(scroll_handler handler) = 0;
        virtual void register_resize_handler(resize_handler handler) = 0;
        virtual void register_key_handler(keyboard_handler handler) = 0;

        virtual misc::size get_view_size() = 0;

        misc::size get_size();
        ::renderer::renderer* get_renderer();

    protected:
        misc::size m_size;
        std::unique_ptr<::renderer::renderer> m_renderer;
    };
} // namespace renderer
