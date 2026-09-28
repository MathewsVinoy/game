#include "engine/input/keyboard_controller.hpp"

namespace engine
{
    glm::vec3 KeyboardMovementController::getMovementVector() const
    {
        glm::vec3 movement{0.0f};
        if (getKeyState(keys.characterMoveForward))
            movement.z += 1.0f;
        if (getKeyState(keys.characterMoveBackward))
            movement.z -= 1.0f;
        if (getKeyState(keys.characterMoveRight))
            movement.x += 1.0f;
        if (getKeyState(keys.characterMoveLeft))
            movement.x -= 1.0f;

        if (glm::dot(movement, movement) > 0.0f)
            return glm::normalize(movement);

        return movement;
    }

    void KeyboardMovementController::onKey(int key, int action)
    {
        if (key < 0 || key >= kMaxKeys)
            return;
        if (action == 1)
        {
            down_[key] = true;
            pressed_[key] = true;
        }
        else if (action == 0)
        {
            down_[key] = false;
        }
    }
}