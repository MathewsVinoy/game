#pragma once

#include "engine/render/devices.hpp"

#include <string>
#include <vector>

namespace engine
{
    struct PipelineConfigInfo
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline
    {
    public:
        Pipeline(
            EngineDevice &device,
            const std::string &vertFilepath,
            const std::string &fragFilepath,
            const PipelineConfigInfo &configInfo);

        ~Pipeline();

        Pipeline(const Pipeline &) = delete;
        void operator=(const Pipeline &) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

    private:
        static std::vector<char> readShaderFile(const std::string &filepath);

        void createGraphicsPipeline(
            const std::string &vertFilepath,
            const std::string &fragFilepath,
            const PipelineConfigInfo &configinfo);

        void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

        EngineDevice &engineDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}