#include "engine/core/application.hpp"
#include "engine/core/render_system.hpp"
#include "engine/render/camera.hpp"
#include "engine/core/keyboard_controller.hpp"
#include "engine/render/buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>
#include <numeric>

namespace engine
{
    struct GlobalUbo
    {
        glm::mat4 projectionView{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f}; // w is intensity
        glm::vec3 lightPosition{-1.f};
        alignas(16) glm::vec4 lightColor{1.f};
    };

    Application::Application()
    {
        globalPool =
            DescriptorPool::Builder(engineDevice)
                .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();
        loadGameObjects();
    }

    Application::~Application() {}

    void Application::run()
    {
        std::vector<std::unique_ptr<EngineBuffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<EngineBuffer>(
                engineDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout =
            DescriptorSetLayout::Builder(engineDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        RenderSystem renderSystem{
            engineDevice,
            renderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout()};
        // RenderSystem renderSystem{engineDevice, renderer.getSwapChainRenderPass()};
        Camera camera{};

        auto viewerObject = GameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!window.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime =
                std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

            if (auto commandBuffer = renderer.beginFrame())
            {
                int frameIndex = renderer.getFrameIndex();
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera};
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                renderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(frameInfo, gameObjects);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(engineDevice.device());
    }

    void Application::loadGameObjects()
    {
        std::shared_ptr<ModelBuffer> model =
            ModelBuffer::createModelFromFile(engineDevice, "assets/models/flat_vase.obj");
        auto gameObject = GameObject::createGameObject();
        gameObject.modelBuffer = model;
        gameObject.transform.translation = {.5f, .5f, 0.f};
        // gameObject.transform.rotation = {3.f, 1.5f, 3.f};
        gameObject.transform.scale = glm::vec3{3.f};

        gameObjects.push_back(std::move(gameObject));

        model = ModelBuffer::createModelFromFile(engineDevice, "assets/models/quad.obj");
        auto floor = GameObject::createGameObject();
        floor.modelBuffer = model;
        floor.transform.translation = {.0f, .5f, 0.0f};
        floor.transform.scale = glm::vec3{3.f, 1.5f, 3.f};
        gameObjects.push_back(std::move(floor));
    }

} // namespace engine