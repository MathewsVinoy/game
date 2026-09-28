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
#include <cmath>
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
            glm::radians(60.0f),
            renderer.getAspectRatio(),
            0.1f,
            1000.0f);

        camera.setViewTarget(
            glm::vec3{0.0f, 1.5f, 8.0f}, // camera position
            glm::vec3{0.0f, 0.5f, 0.0f}, // target/player position
            glm::vec3{0.0f, 1.0f, 0.0f}  // up direction
        );

        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        keyboardController.setWindow(window.getGLFWwindow());

        int windowWidth = 0;
        int windowHeight = 0;
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);
        const double centerX = static_cast<double>(windowWidth) * 0.5;
        const double centerY = static_cast<double>(windowHeight) * 0.5;
        glfwSetCursorPos(window.getGLFWwindow(), centerX, centerY);
        mouseInitialized = true;

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

            auto playerIt = std::find_if(gameObjects.begin(), gameObjects.end(), [](const auto &entry)
                                         { return entry.second.animatedModel != nullptr; });
            if (playerIt != gameObjects.end())
            {
                auto &player = playerIt->second;
                glm::vec3 moveInput = keyboardController.getMovementVector();
                const bool isMoving = glm::dot(moveInput, moveInput) > 0.0f;

                glm::vec3 forward =
                    glm::normalize(glm::vec3(std::sin(mouseYaw), 0.0f, std::cos(mouseYaw)));
                glm::vec3 right =
                    glm::normalize(glm::vec3(std::cos(mouseYaw), 0.0f, -std::sin(mouseYaw)));
                glm::vec3 worldMove = forward * moveInput.z + right * moveInput.x;

                if (isMoving)
                {
                    const float moveSpeed = 5.0f;
                    player.transform.rotation.y = std::atan2(worldMove.x, worldMove.z);
                    player.transform.translation += worldMove * moveSpeed * frameTime;
                }

                if (animatedCharacter)
                {
                    animatedCharacter->setMovement(isMoving);
                }

                const glm::vec3 playerPosition = player.transform.translation;
                const glm::vec3 cameraOffset = glm::vec3{
                    std::sin(mouseYaw) * 6.0f,
                    2.5f + std::sin(mousePitch) * 4.0f,
                    std::cos(mouseYaw) * -6.0f};
                const glm::vec3 cameraTarget = playerPosition + glm::vec3{0.0f, 1.5f, 0.0f};
                const glm::vec3 cameraPosition = playerPosition + cameraOffset;
                camera.setViewTarget(cameraPosition, cameraTarget, glm::vec3{0.0f, 1.0f, 0.0f});
            }

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
        ground.color = glm::vec3(0.0f);
        ground.isGround = true; // alpha=0 triggers checkerboard in shader
        gameObjects.emplace(ground.getId(), std::move(ground));

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