#include "engine/core/application.hpp"
#include "engine/core/render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace engine
{

    Application::Application()
    {
        loadGameObjects();
    }

    Application::~Application() {}

    void Application::run()
    {
        RenderSystem renderSystem{engineDevice, renderer.getSwapChainRenderPass()};
        while (!window.shouldClose())
        {
            glfwPollEvents();
            if (auto commandBuffer = renderer.beginFrame())
            {
                renderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameObjects);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(engineDevice.device());
    }

    void Application::loadGameObjects()
    {
        std::vector<ModelBuffer::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                                  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                                  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
        auto modelBuffer = std::make_shared<ModelBuffer>(engineDevice, vertices);

        auto triangle = GameObject::createGameObject();
        triangle.modelBuffer = modelBuffer;
        triangle.color = {0.1f, 0.79f, 0.1f};
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = {2.0f, 0.5f};
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

} // namespace engine