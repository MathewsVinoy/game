#pragma once

#include "engine/render/devices.hpp"
#include "engine/render/buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace engine
{
  class ModelBuffer
  {
  public:
    struct Vertex
    {
      glm::vec3 position{};
      glm::vec3 color{};
      glm::vec3 normal{};
      glm::vec2 uv{};

      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

      bool operator==(const Vertex &other) const
      {
        return position == other.position && color == other.color && normal == other.normal &&
               uv == other.uv;
      }
    };

    struct Builder
    {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};

      void loadModel(const std::string &filepath);
    };

    ModelBuffer(EngineDevice &device, const ModelBuffer::Builder &builder);
    ~ModelBuffer();

    ModelBuffer(const ModelBuffer &) = delete;
    ModelBuffer &operator=(const ModelBuffer &) = delete;

    static std::unique_ptr<ModelBuffer> createModelFromFile(
        EngineDevice &device, const std::string &filepath);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);

    EngineDevice &engineDevice;

    std::unique_ptr<EngineBuffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexBuffer = false;
    std::unique_ptr<EngineBuffer> indexBuffer;
    uint32_t indexCount;
  };
} // namespace engine