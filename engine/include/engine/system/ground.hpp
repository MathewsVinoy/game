#pragma once

#include "engine/core/frame_info.hpp"
#include "engine/render/buffer.hpp"
#include "engine/render/pipeline.hpp"

#include <memory>
#include <vector>

namespace engine
{
    class Ground
    {
    public:
        Ground(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~Ground();

        Ground(const Ground &) = delete;
        Ground &operator=(const Ground &) = delete;

        void render(FrameInfo &frameInfo);

    private:
        struct PushConstantData
        {
            glm::mat4 modelMatrix{1.f};
            glm::mat4 normalMatrix{1.f};
        };

        struct Vertex
        {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);
        void createVertexBuffer();
        void createIndexBuffer();

        EngineDevice &engineDevice;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::unique_ptr<EngineBuffer> vertexBuffer;
        std::unique_ptr<EngineBuffer> indexBuffer;
        uint32_t indexCount = 0;

        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
}