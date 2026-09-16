#include "engine/render/model_buffers.hpp"

#include "engine/render/utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>

#include <cassert>
#include <cstring>
#include <unordered_map>

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

namespace engine
{

    ModelBuffer::ModelBuffer(EngineDevice &device, const ModelBuffer::Builder &builder) : engineDevice{device}
    {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    ModelBuffer::~ModelBuffer() {}

    std::unique_ptr<ModelBuffer> ModelBuffer::createModelFromFile(
        EngineDevice &device, const std::string &filepath)
    {
        Builder builder{};
        if (filepath.ends_with(".gltf") || filepath.ends_with(".glb"))
        {
            builder.loadModelGltf(filepath);
        }
        else
        {
            builder.loadModel(filepath);
        }

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

    std::vector<VkVertexInputAttributeDescription> ModelBuffer::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

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

    void ModelBuffer::Builder::loadModelGltf(const std::string &filepath)
    {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string err, warn;

              // Load GLB or glTF
        bool ret;
        if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".glb")
        {
            ret = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
        }
        else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".gltf")
        {
            ret = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
        }
        else
        {
            throw std::runtime_error("Unsupported file format: " + filepath);
        }

        if (!ret)
        {
            throw std::runtime_error("Failed to load glTF/GLB file: " + filepath + "\nError: " + err);
        }

        vertices.clear();
        indices.clear();
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        // Default values for missing attributes
        glm::vec3 defaultNormal = {0.0f, 1.0f, 0.0f};
        glm::vec2 defaultUV = {0.0f, 0.0f};

        for (const auto &mesh : model.meshes)
        {
            for (const auto &primitive : mesh.primitives)
            {
                // POSITION is required
                auto positionIt = primitive.attributes.find("POSITION");
                if (positionIt == primitive.attributes.end())
                {
                    continue;
                }

                const auto &positionAccessor = model.accessors[positionIt->second];
                const auto &positionBufferView = model.bufferViews[positionAccessor.bufferView];
                const auto &positionBuffer = model.buffers[positionBufferView.buffer];
                const float *positions = reinterpret_cast<const float *>(
                    positionBuffer.data.data() + positionBufferView.byteOffset + positionAccessor.byteOffset);

                // Load normals if they exist
                const float *normals = nullptr;
                auto normalIt = primitive.attributes.find("NORMAL");
                if (normalIt != primitive.attributes.end())
                {
                    const auto &normalAccessor = model.accessors[normalIt->second];
                    const auto &normalBufferView = model.bufferViews[normalAccessor.bufferView];
                    const auto &normalBuffer = model.buffers[normalBufferView.buffer];
                    normals = reinterpret_cast<const float *>(
                        normalBuffer.data.data() + normalBufferView.byteOffset + normalAccessor.byteOffset);
                }

                // Load UVs if they exist
                const float *texcoords = nullptr;
                auto texcoordIt = primitive.attributes.find("TEXCOORD_0");
                if (texcoordIt != primitive.attributes.end())
                {
                    const auto &texcoordAccessor = model.accessors[texcoordIt->second];
                    const auto &texcoordBufferView = model.bufferViews[texcoordAccessor.bufferView];
                    const auto &texcoordBuffer = model.buffers[texcoordBufferView.buffer];
                    texcoords = reinterpret_cast<const float *>(
                        texcoordBuffer.data.data() + texcoordBufferView.byteOffset + texcoordAccessor.byteOffset);
                }

                // Load indices if they exist
                if (primitive.indices >= 0)
                {
                    const auto &indexAccessor = model.accessors[primitive.indices];
                    const auto &indexBufferView = model.bufferViews[indexAccessor.bufferView];
                    const auto &indexBuffer = model.buffers[indexBufferView.buffer];
                    const uint16_t *indicesData = reinterpret_cast<const uint16_t *>(
                        indexBuffer.data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset);

                    for (size_t i = 0; i < indexAccessor.count; ++i)
                    {
                        Vertex vertex{};
                        vertex.position = {
                            positions[indicesData[i] * 3 + 0],
                            positions[indicesData[i] * 3 + 1],
                            positions[indicesData[i] * 3 + 2]};

                        if (normals)
                        {
                            vertex.normal = {
                                normals[indicesData[i] * 3 + 0],
                                normals[indicesData[i] * 3 + 1],
                                normals[indicesData[i] * 3 + 2]};
                        }
                        else
                        {
                            vertex.normal = defaultNormal;
                        }

                        if (texcoords)
                        {
                            vertex.uv = {
                                texcoords[indicesData[i] * 2 + 0],
                                texcoords[indicesData[i] * 2 + 1]};
                        }
                        else
                        {
                            vertex.uv = defaultUV;
                        }

                        if (uniqueVertices.find(vertex) == uniqueVertices.end())
                        {
                            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                            vertices.push_back(vertex);
                        }
                        indices.push_back(uniqueVertices[vertex]);
                    }
                }
                else
                {
                    // No indices: treat as a non-indexed mesh
                    for (size_t i = 0; i < positionAccessor.count; ++i)
                    {
                        Vertex vertex{};
                        vertex.position = {
                            positions[i * 3 + 0],
                            positions[i * 3 + 1],
                            positions[i * 3 + 2]};

                        if (normals)
                        {
                            vertex.normal = {
                                normals[i * 3 + 0],
                                normals[i * 3 + 1],
                                normals[i * 3 + 2]};
                        }
                        else
                        {
                            vertex.normal = defaultNormal;
                        }

                        if (texcoords)
                        {
                            vertex.uv = {
                                texcoords[i * 2 + 0],
                                texcoords[i * 2 + 1]};
                        }
                        else
                        {
                            vertex.uv = defaultUV;
                        }

                        if (uniqueVertices.find(vertex) == uniqueVertices.end())
                        {
                            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                            vertices.push_back(vertex);
                        }
                        indices.push_back(uniqueVertices[vertex]);
                    }
                }
            }
        }
    }

}