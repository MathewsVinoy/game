#include "engine/render/model_buffers.hpp"

#include "engine/render/utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cassert>
#include <cstring>
#include <unordered_map>
#include <iostream>

namespace std
{
  template <>
  struct hash<engine::ModelBuffer::Vertex>
  {
    size_t operator()(engine::ModelBuffer::Vertex const &vertex) const
    {
      size_t seed = 0;
      engine::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
      return seed;
    }
  };
}

namespace
{

  void addBoneData(
      engine::ModelBuffer::Vertex &vertex,
      int boneId,
      float weight)
  {
    for (int i = 0; i < 4; ++i)
    {
      if (vertex.weights[i] == 0.0f)
      {
        vertex.boneIds[i] = boneId;
        vertex.weights[i] = weight;
        return;
      }
    }

    // More than 4 bones affect this vertex.
    // We currently keep only the first 4.
  }

}

namespace engine
{

  ModelBuffer::ModelBuffer(EngineDevice &device, const ModelBuffer::Builder &builder) : engineDevice{device}
  {
    skinned = builder.skinned;

    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
  }

  ModelBuffer::~ModelBuffer() {}

  std::unique_ptr<ModelBuffer> ModelBuffer::createModelFromFile(
      EngineDevice &device, const std::string &filepath)
  {
    Builder builder{};

    builder.loadModel(filepath);

    return std::make_unique<ModelBuffer>(device, builder);
  }
  std::unique_ptr<ModelBuffer>
  ModelBuffer::setbulder(EngineDevice &device, std::vector<Vertex> vertices,
                         std::vector<uint32_t> indices)
  {
    Builder builder{};
    builder.vertices = vertices;
    builder.indices = indices;
    return std::make_unique<ModelBuffer>(device, builder);
  }

  void ModelBuffer::createVertexBuffers(const std::vector<Vertex> &vertices)
  {
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3");
    assert(!vertices.empty() && "Vertices cannot be empty");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);

    EngineBuffer stagingBuffer(
        engineDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)vertices.data());

    vertexBuffer = std::make_unique<EngineBuffer>(
        engineDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    engineDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
  }

  void ModelBuffer::createIndexBuffers(const std::vector<uint32_t> &indices)
  {
    indexCount = static_cast<uint32_t>(indices.size());
    hasIndexBuffer = indexCount > 0;
    if (!hasIndexBuffer)
    {
      return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
    uint32_t indexSize = sizeof(indices[0]);

    EngineBuffer stagingBuffer(
        engineDevice,
        indexSize,
        indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)indices.data());

    indexBuffer = std::make_unique<EngineBuffer>(
        engineDevice,
        indexSize,
        indexCount,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    engineDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
  }

  void ModelBuffer::draw(VkCommandBuffer commandBuffer)
  {
    if (hasIndexBuffer)
    {
      vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    }
    else
    {
      vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
  }

  void ModelBuffer::bind(VkCommandBuffer commandBuffer)
  {
    VkBuffer buffers[] = {vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (hasIndexBuffer)
    {
      vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
  }

  std::vector<VkVertexInputBindingDescription> ModelBuffer::Vertex::getBindingDescriptions()
  {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
  }

  std::vector<VkVertexInputAttributeDescription>
  ModelBuffer::Vertex::getAttributeDescriptions()
  {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

    attributeDescriptions.push_back(
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});

    attributeDescriptions.push_back(
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});

    attributeDescriptions.push_back(
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});

    attributeDescriptions.push_back(
        {3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

    // Bone IDs
    attributeDescriptions.push_back(
        {4, 0, VK_FORMAT_R32G32B32A32_SINT, offsetof(Vertex, boneIds)});

    // Bone weights
    attributeDescriptions.push_back(
        {5, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, weights)});

    return attributeDescriptions;
  }

  void ModelBuffer::Builder::loadModel(const std::string &filepath)
  {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
    {
      throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto &shape : shapes)
    {
      for (const auto &index : shape.mesh.indices)
      {
        Vertex vertex{};

        if (index.vertex_index >= 0)
        {
          vertex.position = {
              attrib.vertices[3 * index.vertex_index + 0],
              attrib.vertices[3 * index.vertex_index + 1],
              attrib.vertices[3 * index.vertex_index + 2],
          };
          vertex.color = {
              attrib.colors[3 * index.vertex_index + 0],
              attrib.colors[3 * index.vertex_index + 1],
              attrib.colors[3 * index.vertex_index + 2],
          };
        }

        if (index.normal_index >= 0)
        {
          vertex.normal = {
              attrib.normals[3 * index.normal_index + 0],
              attrib.normals[3 * index.normal_index + 1],
              attrib.normals[3 * index.normal_index + 2],
          };
        }

        if (index.texcoord_index >= 0)
        {
          vertex.uv = {
              attrib.texcoords[2 * index.texcoord_index + 0],
              attrib.texcoords[2 * index.texcoord_index + 1],
          };
        }

        if (uniqueVertices.count(vertex) == 0)
        {
          uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
          vertices.push_back(vertex);
        }
        indices.push_back(uniqueVertices[vertex]);
      }
    }
  }
  void ModelBuffer::Builder::loadAnimatedModel(
      const std::string &filepath,
      const Skeleton &skeleton)
  {
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(
        filepath,
        aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_LimitBoneWeights);

    if (!scene || !scene->mRootNode)
    {
      throw std::runtime_error(
          "Failed to load animated model: " +
          std::string(importer.GetErrorString()));
    }

    vertices.clear();
    indices.clear();

    skinned = true;

    uint32_t vertexOffset = 0;

    for (unsigned int meshIndex = 0;
         meshIndex < scene->mNumMeshes;
         ++meshIndex)
    {
      const aiMesh *mesh =
          scene->mMeshes[meshIndex];

      // ----------------------------------------
      // Vertices
      // ----------------------------------------

      for (unsigned int i = 0;
           i < mesh->mNumVertices;
           ++i)
      {
        Vertex vertex{};

        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z};

        if (mesh->HasNormals())
        {
          vertex.normal = {
              mesh->mNormals[i].x,
              mesh->mNormals[i].y,
              mesh->mNormals[i].z};
        }

        if (mesh->HasTextureCoords(0))
        {
          vertex.uv = {
              mesh->mTextureCoords[0][i].x,
              mesh->mTextureCoords[0][i].y};
        }

        vertex.color = {
            1.0f,
            1.0f,
            1.0f};

        vertices.push_back(vertex);
      }

      // ----------------------------------------
      // Bone IDs + weights
      // ----------------------------------------

      for (unsigned int boneIndex = 0;
           boneIndex < mesh->mNumBones;
           ++boneIndex)
      {
        const aiBone *bone =
            mesh->mBones[boneIndex];

        // IMPORTANT:
        // For now boneIndex is the mesh-local
        // bone index.
        //
        // We will replace this with your
        // Skeleton's global bone index in the
        // next stage.

        const std::string boneName =
            bone->mName.C_Str();

        const Bone *skeletonBone =
            skeleton.findBone(boneName);

        if (skeletonBone == nullptr)
        {
          std::cerr
              << "Warning: bone not found in skeleton: "
              << boneName
              << std::endl;

          continue;
        }

        int id =
            static_cast<int>(
                skeletonBone -
                &skeleton.getBones()[0]);

        for (unsigned int weightIndex = 0;
             weightIndex < bone->mNumWeights;
             ++weightIndex)
        {
          const aiVertexWeight &weight =
              bone->mWeights[weightIndex];

          uint32_t vertexIndex =
              vertexOffset +
              weight.mVertexId;

          if (vertexIndex >= vertices.size())
          {
            continue;
          }

          addBoneData(
              vertices[vertexIndex],
              id,
              weight.mWeight);
        }
      }

      // ----------------------------------------
      // Indices
      // ----------------------------------------

      for (unsigned int faceIndex = 0;
           faceIndex < mesh->mNumFaces;
           ++faceIndex)
      {
        const aiFace &face =
            mesh->mFaces[faceIndex];

        for (unsigned int i = 0;
             i < face.mNumIndices;
             ++i)
        {
          indices.push_back(
              vertexOffset +
              face.mIndices[i]);
        }
      }

      vertexOffset =
          static_cast<uint32_t>(vertices.size());
    }
  }
}