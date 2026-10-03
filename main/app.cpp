

#include "app.hpp"

namespace graphics
{
    void HelloTriangleApplication::run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
    void HelloTriangleApplication::initWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Open Game", nullptr, nullptr);
        // GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        // const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        // glfwSetWindowMonitor(
        // window, monitor,
        // 0, 0,
        // mode->width, mode->height,
        // mode->refreshRate);
    }
    void HelloTriangleApplication::initVulkan() {}
    void HelloTriangleApplication::mainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
    }
    void HelloTriangleApplication::cleanup()
    {
        glfwDestroyWindow(window);

        glfwTerminate();
    }

}