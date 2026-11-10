#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace graphics
{
    class Mouse
    {
    public:
        static void update(GLFWwindow *window);
        static glm::vec2 getMouseOffset();
        static void setFirstMouse(bool first);
        static bool leftMouseButtonPressed(GLFWwindow *window) { return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS; };

    private:
        static double lastX, lastY;
        static double xOffset, yOffset;
        static bool firstMouse;
    };
}