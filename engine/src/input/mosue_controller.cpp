#include "engine/input/mouse_controller.hpp"

namespace engine
{

    double Mouse::lastX = 400.0;
    double Mouse::lastY = 300.0;
    double Mouse::xOffset = 0.0;
    double Mouse::yOffset = 0.0;
    bool Mouse::firstMouse = true;

    void Mouse::update(GLFWwindow *window)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        xOffset = xpos - lastX;
        yOffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
    }

    glm::vec2 Mouse::getMouseOffset()
    {
        return glm::vec2(xOffset, yOffset);
    }

    void Mouse::setFirstMouse(bool first)
    {
        firstMouse = first;
    }

}