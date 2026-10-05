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
        void createSurface();
        void createLogicalDevice();

        void setupDebugMessenger();

        GLFWwindow *window;
        VkInstance instance;
        VkDevice device;
        VkQueue graphicsQueue;
        VkDebugUtilsMessengerEXT debugMessenger;

        const uint32_t WIDTH = 800;
        const uint32_t HEIGHT = 600;
    };
}