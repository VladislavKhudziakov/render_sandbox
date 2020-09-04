


#pragma once

#include <renderer/gl/buffer.hpp>
#include <renderer/renderer.hpp>

namespace renderer::gl
{
    class parameters_list
    {
        friend class renderer;

    public:
        explicit parameters_list(const ::renderer::parameters_list_descriptor&);
        void set_parameter_data(uint32_t parameter_index, void*);
        void load_data_to_gpu();
        const std::string& get_name();

    private:
        struct parameter
        {
            size_t offset;
            size_t size;
        };

        std::vector<parameter> m_parameters;
        std::vector<uint8_t> m_parameters_data;
        std::optional<uniform_buffer> m_gpu_storage;

        std::string m_name;
    };
} // namespace renderer::gl
