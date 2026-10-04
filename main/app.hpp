#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace graphics
{
    class HelloTriangleApplication
    {
    public:
        void run();

    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();
        void createInstance();

        GLFWwindow *window;
        VkInstance instance;
        const uint32_t WIDTH = 800;
        const uint32_t HEIGHT = 600;
    };
}