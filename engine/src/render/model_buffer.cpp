#include "engine/render/model_buffers.hpp"

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
} // namespace engine
