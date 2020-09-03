


#pragma once

namespace renderer::gl
{
    template<typename T>
    class bind_guard
    {
    public:
        explicit bind_guard(T& val)
            : m_val(val)
        {
            bind();
        }

        bind_guard(const bind_guard&) = delete;
        bind_guard& operator=(const bind_guard&) = delete;
        bind_guard(bind_guard&&) = delete;
        bind_guard& operator=(bind_guard&&) = delete;

        ~bind_guard()
        {
            unbind();
        }

        void unbind()
        {
            m_val.unbind();
            m_is_bounded = false;
        }

        void bind()
        {
            m_val.bind();
            m_is_bounded = true;
        }

    private:
        T& m_val;
        bool m_is_bounded = false;
    };
} // namespace renderer::gl
