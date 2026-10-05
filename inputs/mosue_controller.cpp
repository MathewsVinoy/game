#include "mouse_controller.hpp"

namespace engine
{
    double Mouse::lastX = 400.0;
    double Mouse::lastY = 300.0;

    double Mouse::xOffset = 0.0;
    double Mouse::yOffset = 0.0;

    bool Mouse::firstMouse = true;

    void Mouse::update(GLFWwindow *window)
    {
        double xpos;
        double ypos;

        glfwGetCursorPos(
            window,
            &xpos,
            &ypos);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;

            firstMouse = false;

            // Don't create a large movement on the first frame.
            xOffset = 0.0;
            yOffset = 0.0;

            return;
        }

        xOffset = xpos - lastX;
        yOffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;
    }

    glm::vec2 Mouse::getMouseOffset()
    {
        glm::vec2 offset{
            static_cast<float>(xOffset),
            static_cast<float>(yOffset)};

        // Consume the mouse movement.
        xOffset = 0.0;
        yOffset = 0.0;

        return offset;
    }

    void Mouse::setFirstMouse(bool first)
    {
        firstMouse = first;
    }

} // namespace engine
