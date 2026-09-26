#pragma once

#include "engine/render/camera.hpp"
#include "engine/render/object.hpp"

#include <vulkan/vulkan.h>

namespace engine
{

#define MAX_LIGHTS 10

    struct Sunlight
    {
        glm::vec4 direction{0.f, 0.f, 0.f, 0.f}; // std140 alignment: vec3 + padding
        glm::vec4 color{1.f, 1.f, 1.f, 1.f};     // RGB color + intensity (w)
    };

    struct GlobalUbo
    {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::mat4 inverseView{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f}; // w is intensity
        Sunlight sunlight{};
        int numLights{0};
    };

    struct BoneUbo
    {
        glm::mat4 bones[128];
    };

    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        GameObject::Map &gameObjects;
    };
}