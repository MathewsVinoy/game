
#pragma once

#include <cstdint>
#include <string>

namespace graphics
{

    struct ImageData
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char *pixels = nullptr;
    };

    ImageData loadImage(const std::string &path);
    void freeImage(ImageData &image);

} // namespace graphics
