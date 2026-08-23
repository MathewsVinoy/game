#include "engine/core/application.hpp"

#include <stdexcept>

namespace engine
{
    Application::Application()
    {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    Application::~Application() { vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr); }

    void Application::run()
    {
        while (!window.shouldClose())
        {
            glfwPollEvents();
        }
    }

    void Application::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void Application::createPipeline()
    {
        auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(swapChain.width(), swapChain.height());
        pipelineConfig.renderPass = swapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
            engineDevice,
            "engine/shaders/triangle.vert.spv",
            "engine/shaders/triangle.frag.spv",
            pipelineConfig);
    }

    void Application::createCommandBuffers() {}
    void Application::drawFrame() {}
}