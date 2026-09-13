#include "engine/system/ground.hpp"

#include <cassert>
#include <stdexcept>

namespace engine
{
    std::vector<VkVertexInputBindingDescription> Ground::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Ground::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
        return attributeDescriptions;
    }

    Ground::Ground(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        : engineDevice{device}
    {
        vertices = {
            {{-50.0f, 0.0f, -50.0f}, {0.2f, 0.6f, 0.2f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{50.0f, 0.0f, -50.0f}, {0.2f, 0.6f, 0.2f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{50.0f, 0.0f, 50.0f}, {0.2f, 0.6f, 0.2f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-50.0f, 0.0f, 50.0f}, {0.2f, 0.6f, 0.2f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        };
        indices = {0, 1, 2, 0, 2, 3};
        indexCount = static_cast<uint32_t>(indices.size());

        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
        createVertexBuffer();
        createIndexBuffer();
    }

    Ground::~Ground()
    {
        vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
    }

    void Ground::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create ground pipeline layout");
        }
    }

    void Ground::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create ground pipeline before pipeline layout");

        PipelineConfigInfo config{};
        Pipeline::defaultPipelineConfigInfo(config);
        config.bindingDescriptions = Vertex::getBindingDescriptions();
        config.attributeDescriptions = Vertex::getAttributeDescriptions();
        config.renderPass = renderPass;
        config.pipelineLayout = pipelineLayout;

        pipeline = std::make_unique<Pipeline>(
            engineDevice,
            "engine/shaders/ground.vert.spv",
            "engine/shaders/ground.frag.spv",
            config);
    }

    void Ground::createVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        uint32_t vertexSize = sizeof(vertices[0]);

        EngineBuffer stagingBuffer(
            engineDevice,
            vertexSize,
            static_cast<uint32_t>(vertices.size()),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.map();
        stagingBuffer.writeToBuffer(vertices.data());

        vertexBuffer = std::make_unique<EngineBuffer>(
            engineDevice,
            vertexSize,
            static_cast<uint32_t>(vertices.size()),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        engineDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    }

    void Ground::createIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
        uint32_t indexSize = sizeof(indices[0]);

        EngineBuffer stagingBuffer(
            engineDevice,
            indexSize,
            static_cast<uint32_t>(indices.size()),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.map();
        stagingBuffer.writeToBuffer(indices.data());

        indexBuffer = std::make_unique<EngineBuffer>(
            engineDevice,
            indexSize,
            static_cast<uint32_t>(indices.size()),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        engineDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
    }

    void Ground::render(FrameInfo &frameInfo)
    {
        pipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        VkBuffer vertexBuffers[] = {vertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(frameInfo.commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

        PushConstantData push{};
        push.modelMatrix = glm::mat4(1.0f);
        push.normalMatrix = glm::mat4(1.0f);

        vkCmdPushConstants(
            frameInfo.commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(PushConstantData),
            &push);

        vkCmdDrawIndexed(frameInfo.commandBuffer, indexCount, 1, 0, 0, 0);
    }

} // namespace engine