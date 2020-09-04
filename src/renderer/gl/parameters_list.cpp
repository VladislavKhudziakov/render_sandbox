

#include "parameters_list.hpp"

#include <renderer/gl/traits.hpp>


renderer::gl::parameters_list::parameters_list(const ::renderer::parameters_list_descriptor& descriptor)
    : m_name(descriptor.name)
{
    m_parameters.reserve(descriptor.parameters.size());

    size_t storage_size = 0;

    for (const auto param : descriptor.parameters) {
        const auto param_size = traits::get_parameter_size(param);
        auto& storage = m_parameters.emplace_back();
        storage.offset = storage_size;
        storage.size = param_size;
        storage_size += param_size;
    }

    m_parameters_data.resize(storage_size);
    m_gpu_storage.emplace(storage_size);
}


void renderer::gl::parameters_list::set_parameter_data(uint32_t parameter_index, void* data)
{
    const auto& param = m_parameters[parameter_index];
    std::memcpy(m_parameters_data.data(), data, param.size);
}


void renderer::gl::parameters_list::load_data_to_gpu()
{
    if (m_gpu_storage.has_value()) {
        bind_guard bind(m_gpu_storage.value());
        m_gpu_storage->load_data(m_parameters_data.data());
    }
}


const std::string& renderer::gl::parameters_list::get_name()
{
    return m_name;
}
