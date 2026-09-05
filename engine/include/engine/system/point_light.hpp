#pragma once

#include "engine/render/pipeline.hpp"
#include "engine/render/devices.hpp"
#include "engine/render/object.hpp"
#include "engine/render/camera.hpp"
#include "engine/core/frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace engine
{
    class PointLightSystem
    {
    public:
        PointLightSystem(
            EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem &) = delete;
        PointLightSystem &operator=(const PointLightSystem &) = delete;

        void update(FrameInfo &frameInfo, GlobalUbo &ubo);
        void render(FrameInfo &frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        EngineDevice &engineDevice;

        std::unique_ptr<Pipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
    };
}