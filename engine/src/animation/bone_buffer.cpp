#include "engine/animation/bone_buffer.hpp"

#include <algorithm>
#include <cstring>

namespace engine
{

    BoneBuffer::BoneBuffer(
        EngineDevice &device,
        const std::vector<glm::mat4> &boneMatrices)
        : device{device}
    {
        buffer = std::make_unique<EngineBuffer>(
            device,
            sizeof(glm::mat4),
            MAX_BONES,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        buffer->map();

        update(boneMatrices);
    }

    VkDescriptorBufferInfo BoneBuffer::descriptorInfo() const
    {
        return buffer->descriptorInfo(
            sizeof(glm::mat4) * MAX_BONES);
    }

    void BoneBuffer::update(
        const std::vector<glm::mat4> &boneMatrices)
    {
        std::vector<glm::mat4> matrices(MAX_BONES, glm::mat4{1.0f});

        const size_t count =
            std::min(
                boneMatrices.size(),
                static_cast<size_t>(MAX_BONES));

        std::copy(
            boneMatrices.begin(),
            boneMatrices.begin() + count,
            matrices.begin());

        buffer->writeToBuffer(
            matrices.data(),
            sizeof(glm::mat4) * MAX_BONES);
    }

}