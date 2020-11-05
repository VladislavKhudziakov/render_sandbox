

#pragma once

#include <memory/pool.hpp>
#include <misc/types_traits.hpp>
#include <misc/debug.hpp>

namespace memory
{
    template<typename ObjectType>
    class pool_view
    {
    public:
        using iterator = ObjectType*;
        using const_iterator = const ObjectType*;

        explicit pool_view(pool<ObjectType>* pool)
            : m_pool(pool)
        {
        }

        ObjectType& operator[](size_t i)
        {
            ASSERT(m_pool != nullptr);
            ASSERT(!m_pool->is_id_expired(i));
            return m_pool->operator[](i);
        }

        const ObjectType& operator[](size_t i) const
        {
            ASSERT(m_pool != nullptr);
            ASSERT(!m_pool->is_id_expired(i));
            return m_pool->operator[](i);
        }

        pool<ObjectType>* get_pool()
        {
            return m_pool;
        }

        bool has_pool() const
        {
            return m_pool != nullptr;
        }

        size_t size() const
        {
            return m_pool->end() - m_pool->begin();
        }

        iterator begin()
        {
            if (m_pool == nullptr) {
                return nullptr;
            } else {
                return m_pool->begin();
            }
        }

        iterator end()
        {
            if (m_pool == nullptr) {
                return nullptr;
            } else {
                return m_pool->end();
            }
        }

        const_iterator cbegin() const
        {
            if (m_pool == nullptr) {
                return nullptr;
            } else {
                return m_pool->begin();
            }
        }

        const_iterator cend() const
        {
            if (m_pool == nullptr) {
                return nullptr;
            } else {
                return m_pool->end();
            }
        }

    private:
        pool<ObjectType>* m_pool;
    };

    template<typename Registrator>
    class pool_factory
    {
    public:
        inline static size_t pools_size{0};

        template<typename ObjectType>
        size_t get_type_id() const
        {
            using T = std::remove_reference_t<std::remove_cv_t<ObjectType>>;
            return misc::type_traits::type_id<pool_factory>::template get<T>();
        }

        template<typename ObjectType, typename... Args>
        size_t create(Args&&... args)
        {
            using T = std::remove_reference_t<std::remove_cv_t<ObjectType>>;
            const auto index = get_type_id<T>();

            if (index >= m_pools_list.size()) {
                m_pools_list.resize(index + 1);
            }

            if (m_pools_list[index] == nullptr) {
                m_pools_list[index] = pool_wrapper<T>::create();
            }

            return m_pools_list[index]->template as_typed_wrapper<T>()->pool.create(std::forward<Args>(args)...);
        }

        template<typename ObjectType>
        void destroy(size_t id)
        {
            auto* pool = get_pool<ObjectType>();

            if (pool == nullptr) {
                return;
            }

            pool->destroy(id);
        }

        void destroy(size_t type_id, size_t id)
        {
            if (type_id >= m_pools_list.size()) {
                return;
            }

            auto& pool = m_pools_list[type_id];

            if (pool == nullptr) {
                return;
            }

            pool->destroy(id);
        }

        template<typename ObjectType>
        pool_view<ObjectType> view() const
        {
            return pool_view<ObjectType>{get_pool<ObjectType>()};
        }

    private:
        template<typename Object>
        struct pool_wrapper;

        struct pool_wrapper_base
        {
            virtual ~pool_wrapper_base() = default;

            template<typename T>
            pool_wrapper<T>* as_typed_wrapper()
            {
                return static_cast<pool_wrapper<T>*>(this);
            }

            virtual void destroy(size_t) = 0;
        };

        template<typename Object>
        struct pool_wrapper : pool_wrapper_base
        {
            static std::unique_ptr<pool_wrapper_base> create()
            {
                return std::make_unique<pool_wrapper>();
            }

            void destroy(size_t id) override
            {
                pool.destroy(id);
            }

            pool<Object> pool;
        };

        template<typename ObjectType>
        pool<ObjectType>* get_pool() const
        {
            using T = std::remove_reference_t<std::remove_cv_t<ObjectType>>;
            const auto index = misc::type_traits::type_id<pool_factory>::template get<T>();

            if (index >= m_pools_list.size()) {
                return nullptr;
            }

            if (m_pools_list[index] == nullptr) {
                return nullptr;
            }

            return &m_pools_list[index]->template as_typed_wrapper<T>()->pool;
        }

        std::vector<std::unique_ptr<pool_wrapper_base>> m_pools_list;
    };
} // namespace memory
