
#include "image_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>
#include <string>

namespace graphics
{

    ImageData loadImage(const std::string &path)
    {
        ImageData image;

        // Force a consistent RGBA pixel format.
        image.pixels = stbi_load(
            path.c_str(),
            &image.width,
            &image.height,
            &image.channels,
            STBI_rgb_alpha);

        if (!image.pixels)
        {
            throw std::runtime_error(
                "Failed to load image: " + path +
                " (" + stbi_failure_reason() + ")");
        }

        // Pixels are now RGBA, four bytes per pixel.
        image.channels = 4;

        return image;
    }

    void freeImage(ImageData &image)
    {
        if (image.pixels)
        {
            stbi_image_free(image.pixels);
        }

        image.pixels = nullptr;
        image.width = 0;
        image.height = 0;
        image.channels = 0;
    }

} // namespace graphics
