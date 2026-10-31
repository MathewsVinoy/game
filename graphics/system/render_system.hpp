#pragma once

#include "../frame_info.h"
#include "graphics/render/camera.hpp"
#include "graphics/core/devices.hpp"
#include "object/object.hpp"
#include "graphics/render/pipeline.hpp"

#include <memory>
#include <vector>

namespace graphics
{
  class RenderSystem
  {
  public:
    RenderSystem(
        EngineDevice &engineDevice,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout,
        VkDescriptorSetLayout boneSetLayout,
        VkDescriptorSetLayout textureSetLayout,
        VkDescriptorSet defaultTextureSet = VK_NULL_HANDLE,
        VkDescriptorSet defaultBoneSet = VK_NULL_HANDLE);

    ~RenderSystem();

    RenderSystem(const RenderSystem &) = delete;
    RenderSystem &operator=(const RenderSystem &) = delete;

    void renderGameObjects(FrameInfo &frameInfo);

  private:
    void createPipelineLayout(
        VkDescriptorSetLayout globalSetLayout,
        VkDescriptorSetLayout boneSetLayout,
        VkDescriptorSetLayout textureSetLayout);

    void createPipeline(VkRenderPass renderPass);

    EngineDevice &engineDevice;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSet defaultTextureSet{VK_NULL_HANDLE};
    VkDescriptorSet defaultBoneSet{VK_NULL_HANDLE};
  };
} // namespace graphics