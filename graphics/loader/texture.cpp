
#include "texture.hpp"

#include "graphics/render/buffer.hpp"
#include "image_loader.hpp"

#include <stdexcept>
#include <array>
#include <limits>
#include <iostream>

namespace graphics
{

    Texture::Texture(EngineDevice &device, const std::string &filepath)
        : engineDevice{device}
    {
        ImageData data = loadImage(filepath);
        std::cout << "Loading texture: " << filepath << '\n';
        std::cout << "Image dimensions: "
                  << data.width << " x " << data.height << '\n';
        std::cout << "Pixel data valid: "
                  << (data.pixels != nullptr) << '\n';

        if (data.pixels != nullptr)
        {
            std::cout << "First pixel RGBA: "
                      << static_cast<int>(data.pixels[0]) << ", "
                      << static_cast<int>(data.pixels[1]) << ", "
                      << static_cast<int>(data.pixels[2]) << ", "
                      << static_cast<int>(data.pixels[3]) << '\n';
        }

        try
        {
            createImage(data.pixels, static_cast<uint32_t>(data.width), static_cast<uint32_t>(data.height));
            freeImage(data);
        }
        catch (...)
        {
            freeImage(data);
            throw;
        }
    }

    Texture::Texture(EngineDevice &device, uint32_t width, uint32_t height, const unsigned char *pixels)
        : engineDevice{device}
    {
        createImage(pixels, width, height);
    }

    void Texture::createImage(
        const unsigned char *pixels,
        uint32_t imageWidth,
        uint32_t imageHeight)
    {
        width = imageWidth;
        height = imageHeight;

        VkDeviceSize imageSize =
            static_cast<VkDeviceSize>(width) * height * 4;

        EngineBuffer stagingBuffer{
            engineDevice,
            imageSize,
            1,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)pixels, imageSize);
        stagingBuffer.unmap();

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage =
            VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        engineDevice.createImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            image,
            imageMemory);

        VkImageMemoryBarrier toTransfer{};
        toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        toTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.image = image;
        toTransfer.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        toTransfer.subresourceRange.baseMipLevel = 0;
        toTransfer.subresourceRange.levelCount = 1;
        toTransfer.subresourceRange.baseArrayLayer = 0;
        toTransfer.subresourceRange.layerCount = 1;
        toTransfer.srcAccessMask = 0;
        toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        VkCommandBuffer commandBuffer =
            engineDevice.beginSingleTimeCommands();

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &toTransfer);

        engineDevice.endSingleTimeCommands(commandBuffer);

        engineDevice.copyBufferToImage(
            stagingBuffer.getBuffer(), image, width, height, 1);

        VkImageMemoryBarrier toShader{};
        toShader.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        toShader.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        toShader.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        toShader.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toShader.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toShader.image = image;
        toShader.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        toShader.subresourceRange.baseMipLevel = 0;
        toShader.subresourceRange.levelCount = 1;
        toShader.subresourceRange.baseArrayLayer = 0;
        toShader.subresourceRange.layerCount = 1;
        toShader.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        toShader.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        commandBuffer = engineDevice.beginSingleTimeCommands();

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &toShader);

        engineDevice.endSingleTimeCommands(commandBuffer);

        createImageView();
        createSampler();
    }

    Texture::~Texture()
    {
        if (sampler != VK_NULL_HANDLE)
            vkDestroySampler(engineDevice.device(), sampler, nullptr);

        if (imageView != VK_NULL_HANDLE)
            vkDestroyImageView(engineDevice.device(), imageView, nullptr);

        if (image != VK_NULL_HANDLE)
            vkDestroyImage(engineDevice.device(), image, nullptr);

        if (imageMemory != VK_NULL_HANDLE)
            vkFreeMemory(engineDevice.device(), imageMemory, nullptr);
    }

    void Texture::createImageView()
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(
                engineDevice.device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture image view");
        }
    }

    void Texture::createSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy =
            engineDevice.properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(
                engineDevice.device(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture sampler");
        }
    }

} // namespace graphics
