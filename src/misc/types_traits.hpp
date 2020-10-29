

#pragma once

#include <type_traits>

namespace misc::type_traits
{
    namespace detail
    {
        struct type_id
        {
            static size_t next()
            {
                static size_t value{0};
                return value++;
            }
        };
    } // namespace detail

    template<typename RegistratorType>
    struct type_id
    {
        template<typename T>
        static size_t get()
        {
            static auto value = detail::type_id::next();
            return value;
        }
    };

} // namespace misc::type_traits
