#include "engine/render/model_buffers.hpp"

#include <cassert>
#include <cstring>

namespace engine
{
    ModelBuffer::ModelBuffer(EngineDevice &engineDevice, const std::vector<Vertex> &vertices) : engineDevice{engineDevice}
    {
        createVertexBuffer(vertices);
    }

    ModelBuffer::~ModelBuffer()
    {
        vkDestroyBuffer(engineDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(engineDevice.device(), vertexBufferMemory, nullptr);
    }

    void ModelBuffer::createVertexBuffer(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        engineDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer,
            vertexBufferMemory);

        void *data;
        vkMapMemory(engineDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(engineDevice.device(), vertexBufferMemory);
    }
} // namespace engine
