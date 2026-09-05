#pragma once

#include "engine/render/pipeline.hpp"
#include "engine/render/devices.hpp"
#include "engine/render/object.hpp"
#include "engine/render/camera.hpp"
#include "engine/core/frame_info.hpp"

#include <memory>
#include <vector>

namespace engine
{
    class RenderSystem
    {
    public:
        RenderSystem(EngineDevice &engineDevice, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
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
}