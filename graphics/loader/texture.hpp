
#pragma once

#include "graphics/core/devices.hpp"

#include <string>

namespace graphics
{

    class Texture
    {
    public:
        Texture(EngineDevice &device, const std::string &filepath);
        ~Texture();

        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;

        VkImageView getImageView() const
        {
            return imageView;
        }

        VkSampler getSampler() const
        {
            return sampler;
        }

        VkDescriptorImageInfo descriptorInfo() const
        {
            return VkDescriptorImageInfo{
                sampler,
                imageView,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        }

    private:
        void createImage(
            const unsigned char *pixels,
            uint32_t width,
            uint32_t height);

        void createImageView();
        void createSampler();

        EngineDevice &engineDevice;

        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;

        uint32_t width = 0;
        uint32_t height = 0;
    };

} // namespace graphics
