#include "engine/core/application.hpp"

#include "engine/render/buffer.hpp"
#include "engine/render/camera.hpp"
#include "engine/system/render_system.hpp"
#include "engine/animation/bone_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace engine
{

    Application::Application()
    {
        globalPool = DescriptorPool::Builder(engineDevice)
                         .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                         .addPoolSize(
                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                             SwapChain::MAX_FRAMES_IN_FLIGHT)
                         .addPoolSize(
                             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                             SwapChain::MAX_FRAMES_IN_FLIGHT)
                         .build();
        loadGameObjects();
    }

    Application::~Application() {}

    void Application::setUpdateCallback(UpdateCallback callback)
    {
        updateCallback = std::move(callback);
    }

    void Application::run()
    {
        std::vector<std::unique_ptr<EngineBuffer>> uboBuffers(
            SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<EngineBuffer>(
                engineDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }
        std::vector<std::unique_ptr<BoneBuffer>> boneBuffers(
            SwapChain::MAX_FRAMES_IN_FLIGHT);
        std::vector<glm::mat4> initialBones(
            BoneBuffer::MAX_BONES,
            glm::mat4{1.0f});
        for (int i = 0; i < boneBuffers.size(); i++)
        {
            boneBuffers[i] =
                std::make_unique<BoneBuffer>(
                    engineDevice,
                    initialBones);
        }
        auto globalSetLayout =
            DescriptorSetLayout::Builder(engineDevice)
                .addBinding(
                    0,
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
                .addBinding(
                    1,
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT)
                .build();
        std::vector<VkDescriptorSet> globalDescriptorSets(
            SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            auto boneBufferInfo = boneBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .writeBuffer(1, &boneBufferInfo)
                .build(globalDescriptorSets[i]);
        }
        RenderSystem renderSystem{engineDevice, renderer.getSwapChainRenderPass(),
                                  globalSetLayout->getDescriptorSetLayout()};

        Camera camera{};
        camera.setPerspectiveProjection(
            glm::radians(50.0f),
            static_cast<float>(Application::WIDTH) /
                static_cast<float>(Application::HEIGHT),
            0.1f,
            100.0f);
        camera.setViewTarget(glm::vec3{0.0f, 1.5f, 8.0f}, glm::vec3{0.0f, 0.5f, 0.0f},
                             glm::vec3{0.0f, 1.0f, 0.0f});
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        auto currentTime = std::chrono::high_resolution_clock::now();
        while (!window.shouldClose())
        {
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime =
                std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                           currentTime)
                    .count();
            currentTime = newTime;
            if (updateCallback)
            {
                updateCallback(frameTime);
            }
            if (animatedCharacter)
            {
                animatedCharacter->update(frameTime);
            }

            if (auto commandBuffer = renderer.beginFrame())
            {
                int frameIndex = renderer.getFrameIndex();
                FrameInfo frameInfo{frameIndex,
                                    frameTime,
                                    commandBuffer,
                                    camera,
                                    globalDescriptorSets[frameIndex],
                                    gameObjects};
                if (!boneBuffers.empty() && animatedCharacter)
                {
                    boneBuffers[frameIndex]->update(
                        animatedCharacter->getAnimator()->getBoneMatrices());
                }

                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                // pointLightSystem.update(frameInfo, ubo);
                const glm::vec3 sunlightDirection =
                    glm::normalize(glm::vec3(0.5f, 1.0f, 0.5f));
                ubo.sunlight.direction = glm::vec4(sunlightDirection, 0.0f);
                ubo.sunlight.color = glm::vec4(1.0f, 0.97f, 0.9f, 2.0f);
                ubo.ambientLightColor = glm::vec4(0.6f, 0.7f, 0.9f, 0.4f); // sky-tinted ambient
                ubo.numLights = 0;
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                // render
                renderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(frameInfo);

                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(engineDevice.device());
    }

    void Application::loadGameObjects()
    {
        // -------------------------------------------------------
        // Ground — flat quad with checkerboard shader (color.a=0)
        // -------------------------------------------------------
        std::vector<ModelBuffer::Vertex> v(4);
        const float h = 100.f;
        const glm::vec3 p[4] = {{-h, 0, -h}, {-h, 0, h}, {h, 0, h}, {h, 0, -h}};
        for (int i = 0; i < 4; i++)
        {
            v[i].position = p[i];
            v[i].normal = {0, 1, 0};
        }
        std::shared_ptr<ModelBuffer> groundModel =
            ModelBuffer::setbulder(
                engineDevice,
                v,
                {0, 1, 2, 0, 2, 3});
        auto ground = GameObject::createGameObject();
        ground.modelBuffer = groundModel;
        ground.color = glm::vec3(0.0f); // a=0 triggers checkerboard in shader
        gameObjects.emplace(ground.getId(), std::move(ground));

        // -------------------------------------------------------
        // Helper: build a simple AABB cube centered at origin
        // -------------------------------------------------------
        auto makeCube = [&](glm::vec3 pos, glm::vec3 scale, glm::vec3 color)
        {
            // 8 corners of a unit cube [-0.5, 0.5]^3
            const glm::vec3 corners[8] = {
                {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
                {0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
                {-0.5f, -0.5f,  0.5f}, {0.5f, -0.5f,  0.5f},
                {0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f},
            };
            // 6 faces: each face 4 vertices with flat normal
            struct FaceDef { int idx[4]; glm::vec3 n; };
            const FaceDef faces[6] = {
                {{0,1,2,3}, { 0, 0,-1}}, // -Z
                {{5,4,7,6}, { 0, 0, 1}}, // +Z
                {{4,0,3,7}, {-1, 0, 0}}, // -X
                {{1,5,6,2}, { 1, 0, 0}}, // +X
                {{3,2,6,7}, { 0, 1, 0}}, // +Y
                {{4,5,1,0}, { 0,-1, 0}}, // -Y
            };
            std::vector<ModelBuffer::Vertex> verts;
            std::vector<uint32_t>            idxs;
            for (const auto& f : faces)
            {
                uint32_t base = static_cast<uint32_t>(verts.size());
                for (int k = 0; k < 4; k++)
                {
                    ModelBuffer::Vertex vtx{};
                    vtx.position = corners[f.idx[k]];
                    vtx.normal   = f.n;
                    verts.push_back(vtx);
                }
                // two triangles per face
                idxs.insert(idxs.end(),
                    {base,base+1,base+2, base,base+2,base+3});
            }
            auto cubeModel = ModelBuffer::setbulder(engineDevice, verts, idxs);
            auto obj = GameObject::createGameObject();
            obj.modelBuffer = std::move(cubeModel);
            obj.color = color;
            obj.transform.translation = pos;
            obj.transform.scale = scale;
            gameObjects.emplace(obj.getId(), std::move(obj));
        };

        // Large cube — left side
        makeCube({-4.5f, 0.75f, -2.0f}, {2.5f, 1.5f, 2.0f}, glm::vec3(0.75f));
        // Small cube — center-right
        makeCube({ 2.5f, 0.4f, -1.0f},  {1.0f, 0.8f, 0.9f}, glm::vec3(0.75f));
        // Medium cube — far right
        makeCube({ 6.5f, 0.6f, -1.5f},  {1.8f, 1.2f, 1.5f}, glm::vec3(0.75f));

        // -------------------------------------------------------
        // Animated character — coral red
        // -------------------------------------------------------
        animatedCharacter = std::make_shared<AnimatedModel>(
            engineDevice,
            "assets/models/Standard Walk.fbx");
        auto character = GameObject::createGameObject();
        character.animatedModel = animatedCharacter;
        character.color = glm::vec3(0.85f, 0.32f, 0.22f); // coral red
        character.transform.translation = {0.0f, 0.0f, 0.0f};
        character.transform.scale = {
            0.01f,
            0.01f,
            0.01f};
        gameObjects.emplace(
            character.getId(),
            std::move(character));
    }

} // namespace engine