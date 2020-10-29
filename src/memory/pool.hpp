

#pragma once

#include <vector>
#include <deque>

namespace memory
{
    template<typename ObjectType, template<typename> typename Allocator = std::allocator>
    class pool
    {
    public:
        pool() = default;

        pool(const pool&) = delete;
        pool& operator=(const pool&) = delete;
        pool(pool&&) = delete;
        pool& operator=(pool&&) = delete;

        ~pool()
        {
            clear();
            m_allocator.deallocate(m_data_pointer, m_storage_size);
        }

        using iterator = ObjectType*;
        using const_iterator = const ObjectType*;

        ObjectType& operator[](size_t index)
        {
            return *std::launder(m_data_pointer + index);
        }

        const ObjectType& operator[](size_t index) const
        {
            return *std::launder(m_data_pointer + index);
        }

        iterator begin()
        {
            return m_data_pointer;
        }

        iterator end()
        {
            return m_data_pointer + m_size;
        }

        template<typename... Args>
        size_t create(Args&&... args) noexcept(std::is_nothrow_move_constructible_v<ObjectType>&& std::is_nothrow_constructible_v<ObjectType, Args...>)
        {
            if (m_free_objects.empty()) {
                assert(m_size <= m_storage_size);

                if (m_size + 1 >= m_storage_size) {
                    reallocate(m_storage_size == 0 ? 10 : m_storage_size * 2);
                }

                new (m_data_pointer + m_size) ObjectType(std::forward<Args>(args)...);
                return m_size++;
            } else {
                const auto id = m_free_objects.back();
                auto object_ptr = m_data_pointer + id;
                new (object_ptr) ObjectType(std::forward<Args>(args)...);
                m_free_objects.pop_back();
                return id;
            }
        }

        ObjectType* object_ptr(size_t id)
        {
            return is_id_expired(id) ? nullptr : std::launder(m_data_pointer + id);
        }

        void destroy(size_t id) noexcept
        {
            if (id >= m_size) {
                return;
            } else {
                m_free_objects.emplace_back(id);
                m_data_pointer[id].~ObjectType();
            }
        }

        std::vector<ObjectType*> objects_view()
        {
            std::vector<ObjectType*> res;

            res.reserve(m_size);

            for (int i = 0; i < m_size; ++i) {
                if (!is_id_expired(i)) {
                    res.emplace_back(std::launder(m_data_pointer + i));
                }
            }

            return res;
        }

        std::vector<const ObjectType*> objects_view() const
        {
            std::vector<ObjectType*> res;

            res.reserve(m_size);

            for (int i = 0; i < m_size; ++i) {
                if (!is_id_expired(i)) {
                    res.emplace_back(std::launder(m_data_pointer + i));
                }
            }

            return res;
        }

        void reserve(size_t n)
        {
            if (n > m_storage_size) {
                reallocate(n);
            }
        }

        void clear()
        {
            for (int i = 0; i < m_size; ++i) {
                if (is_id_expired(i)) {
                    continue;
                }
                std::launder(m_data_pointer + i)->~ObjectType();
                m_free_objects.emplace_back(i);
            }

            m_size = 0;
        }

        bool is_id_expired(size_t id)
        {
            if (id >= m_size) {
                return true;
            }
            return std::find(m_free_objects.begin(), m_free_objects.end(), id) != m_free_objects.end();
        }

    private:
        void reallocate(size_t new_storage_size) noexcept(std::is_nothrow_move_constructible_v<ObjectType> || std::is_nothrow_copy_constructible_v<ObjectType>)
        {
            if (new_storage_size <= m_storage_size) {
                return;
            }

            auto ptr = m_allocator.allocate(new_storage_size);

            size_t i = 0;

            try {
                for (; i < m_size; ++i) {
                    if (is_id_expired(i)) {
                        continue;
                    }

                    if constexpr (std::is_nothrow_move_constructible_v<ObjectType>) {
                        new (ptr + i) ObjectType(std::move(m_data_pointer[i]));
                    } else {
                        new (ptr + i) ObjectType(m_data_pointer[i]);
                    }
                }
            } catch (...) {
                for (size_t j; j < i; ++j) {
                    std::launder(ptr + i)->~ObjectType();
                }
                m_allocator.deallocate(ptr, new_storage_size);
                throw;
            }

            m_allocator.deallocate(m_data_pointer, m_storage_size);
            m_data_pointer = ptr;
            m_storage_size = new_storage_size;
        }

        Allocator<ObjectType> m_allocator{};
        size_t m_storage_size{0};
        size_t m_size{0};
        ObjectType* m_data_pointer{nullptr};
        std::deque<size_t> m_free_objects{};
    };
} // namespace memory
