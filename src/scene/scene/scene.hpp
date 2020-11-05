

#pragma once

#include <memory/pool_factory.hpp>
#include <scene/components/component_base.hpp>

#include <scene/components/transformation/transformation.hpp>

namespace renderer::scene
{
    using object_handler = size_t;

    namespace detail
    {
        template<typename T>
        struct component_wrapper
        {
            component_wrapper(T* ptr, object_handler obj)
                : component_ptr(ptr)
                , object(obj)
            {
            }

            T* component_ptr;
            object_handler object = -1;
        };

        struct component_handler
        {
            size_t type_id;
            size_t handler;
            object_handler object_handler;
        };

        struct object
        {
            std::vector<component_handler> components;
        };
    } // namespace detail

    class scene
    {
    public:
        object_handler create_object()
        {
            auto o = m_pool_factory.create<detail::object>();
            emplace_component<transformation>(o);
            return o;
        }

        void destroy_object(object_handler object_id)
        {
            reset_object(object_id);
            m_pool_factory.destroy<detail::object>(object_id);
        }

        void reset_object(object_handler object_id)
        {
            auto objects_view = m_pool_factory.view<detail::object>();
            auto& obj = objects_view[object_id];

            for (auto c : obj.components) {
                m_pool_factory.destroy(c.type_id, c.handler);
            }
        }

        template<typename T, typename... Args>
        T& emplace_component(object_handler object_id, Args... args)
        {
            static_assert(std::is_base_of_v<component_base, T>, "component must be child of component_base.");
            ASSERT(!has_component<T>(object_id));

            auto& obj = get_object(object_id);
            const auto type_id = m_pool_factory.get_type_id<T>();

            auto component = m_pool_factory.create<T>(std::forward<Args>(args)...);

            obj.components.emplace_back(detail::component_handler{.type_id = type_id, .handler = component, .object_handler = object_id});
            auto& c = m_pool_factory.view<T>()[component];
            c.object_handler = object_id;
            return c;
        }

        template<typename T>
        bool has_component(object_handler object_id) const
        {
            auto& obj = get_object(object_id);

            const auto type_id = m_pool_factory.get_type_id<T>();
            auto it = std::find_if(obj.components.begin(), obj.components.end(), [type_id](detail::component_handler& handler) {
                return handler.type_id == type_id;
            });

            return it != obj.components.end();
        }

        template<typename T>
        T* get_component(object_handler object_id)
        {
            const auto type_id = m_pool_factory.get_type_id<T>();

            auto& obj = get_object(object_id);

            auto it = std::find_if(obj.components.begin(), obj.components.end(), [type_id](detail::component_handler& handler) {
                return handler.type_id == type_id;
            });

            if (it == obj.components.end()) {
                return nullptr;
            }

            auto components_view = m_pool_factory.view<T>();
            auto& c = components_view[it->handler];
            return &c;
        }

        template<typename T>
        void remove_component(object_handler object_id)
        {
            if (!has_component<T>(object_id)) {
                return;
            };

            const auto type_id = m_pool_factory.get_type_id<T>();

            auto& obj = get_object(object_id);

            auto it = std::find_if(obj.components.begin(), obj.components.end(), [type_id](detail::component_handler& handler) {
                return handler.type_id == type_id;
            });

            if (it == obj.components.end()) {
                return;
            }

            m_pool_factory.destroy<T>(it->handler);
            obj.components.erase(it);
        }

        template<typename T>
        auto view()
        {
            auto v = m_pool_factory.view<T>();
            if (v.get_pool() == nullptr) {
                return std::vector<T*>{};
            } else {
                return v.get_pool()->objects_view();
            }
        }

        template<typename T>
        std::vector<object_handler> objects_view()
        {
            auto view = m_pool_factory.view<T>();

            std::vector<object_handler> res;
            res.reserve(view.size());

            for (int i = 0; i < view.size(); ++i) {
                if (view.get_pool()->is_id_expired(i)) {
                    continue;
                }

                res.emplace_back(view[i].object_handler);
            }

            return res;
        }

    private:
        detail::object& get_object(object_handler object_id) const
        {
            auto objects_view = m_pool_factory.view<detail::object>();
            auto& obj = objects_view[object_id];
            return obj;
        }
        memory::pool_factory<scene> m_pool_factory;
    };
} // namespace renderer::scene
