#pragma once

#include "../frame_info.h"
#include "graphics/render/camera.hpp"
#include "graphics/core/devices.hpp"
#include "graphics/render/object.hpp"
#include "graphics/render/pipeline.hpp"

#include <memory>
#include <vector>

namespace engine
{
  class RenderSystem
  {
  public:
    RenderSystem(EngineDevice &engineDevice, VkRenderPass renderPass,
                 VkDescriptorSetLayout globalSetLayout);
    ~RenderSystem();

    RenderSystem(const RenderSystem &) = delete;
    RenderSystem &operator=(const RenderSystem &) = delete;

    void renderGameObjects(FrameInfo &frameInfo);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    EngineDevice &engineDevice;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
  };
} // namespace engine