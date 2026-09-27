#pragma once

#include "engine/render/buffer.hpp"
#include "engine/animation/skeleton.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace engine
{

    class BoneBuffer
    {
    public:
        static constexpr uint32_t MAX_BONES = 128;

        BoneBuffer(
            EngineDevice &device,
            const std::vector<glm::mat4> &boneMatrices);

        ~BoneBuffer() = default;

        BoneBuffer(const BoneBuffer &) = delete;
        BoneBuffer &operator=(const BoneBuffer &) = delete;

        VkDescriptorBufferInfo descriptorInfo() const;

        void update(const std::vector<glm::mat4> &boneMatrices);

    private:
        EngineDevice &device;
        std::unique_ptr<EngineBuffer> buffer;
    };

}