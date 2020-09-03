

#pragma once

#include <cinttypes>
#include <algorithm>


namespace renderer::gl
{
    template<typename CreatePolicy, typename DestroyPolicy>
    class raii_storage
    {
    public:
        raii_storage()
        {
            CreatePolicy::create(&m_handler);
        }

        ~raii_storage()
        {
            if (m_handler != 0) {
                DestroyPolicy::destroy(&m_handler);
            }
        }

        raii_storage(const raii_storage&) = delete;

        raii_storage& operator=(const raii_storage&) = delete;

        raii_storage(raii_storage&& src) noexcept
        {
            *this = std::move(src);
        }

        raii_storage& operator=(raii_storage&& src) noexcept
        {
            if (this != &src) {
                m_handler = src.m_handler;
                src.m_handler = 0;
            }

            return *this;
        }

        operator uint32_t()
        {
            return m_handler;
        }

    protected:
        uint32_t m_handler;
    };
} // namespace renderer::gl
