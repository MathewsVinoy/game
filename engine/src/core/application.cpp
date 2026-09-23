#include "engine/core/application.hpp"

namespace engine
{
    Application::Application() {}

    Application::~Application() {}

    void Application::run()
    {
        while (!window.shouldClose())
        {
            glfwPollEvents();
        }
        vkDeviceWaitIdle(engineDevice.device());
    }
}