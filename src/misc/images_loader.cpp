

#include "images_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include <filesystem>
#include <iostream>
#include <functional>
#include <cstring>

renderer::texture_descriptor misc::images_loader::load_2d_texture(const std::string& file)
{
    renderer::texture_descriptor res;

    int32_t w, h, c;
    std::unique_ptr<stbi_uc, std::function<void(stbi_uc*)>> image(
        stbi_load(file.c_str(), &w, &h, &c, STBI_default), [](stbi_uc* image_ptr) {
            if (image_ptr != nullptr) {
                stbi_image_free(image_ptr);
            }
        });

    if (image == nullptr) {
        throw std::runtime_error("invalid image");
    }

    switch (c) {
        case STBI_grey:
            res.format = renderer::texture_format::r;
            break;
        case STBI_grey_alpha:
            res.format = renderer::texture_format::rg;
            break;
        case STBI_rgb_alpha:
            res.format = renderer::texture_format::rgba;
            break;
        default:
            throw std::runtime_error("invalid image format. Available images formats: R, RG, RGBA.");
    }

    res.type = renderer::texture_type::d2;
    res.pixels_data_type = renderer::data_type::u8;
    res.size.width = w;
    res.size.height = h;

    res.pixels.resize(w * h * c);
    std::memcpy(res.pixels.data(), image.get(), w * h * c);

    return res;
}
