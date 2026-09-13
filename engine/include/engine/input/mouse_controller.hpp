#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace engine
{
    class Mouse
    {
    public:
        static void update(GLFWwindow *window);
        static glm::vec2 getMouseOffset();
        static void setFirstMouse(bool first);

    private:
        static double lastX, lastY;
        static double xOffset, yOffset;
        static bool firstMouse;
    };
}