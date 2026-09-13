#pragma once

#include "engine/render/object.hpp"
#include "engine/core/window.hpp"

namespace engine
{
  class KeyboardMovementController
  {
  public:
    KeyboardMovementController() = default;
    explicit KeyboardMovementController(GLFWwindow *window) : window{window} {}

    struct KeyMappings
    {

      int characterMoveLeft = GLFW_KEY_A;
      int characterMoveRight = GLFW_KEY_D;
      int characterMoveForward = GLFW_KEY_W;
      int characterMoveBackward = GLFW_KEY_S;

      int spacebar = GLFW_KEY_SPACE;
    };

    // void moveInPlaneXZ(GLFWwindow *window, float dt, GameObject &gameObject);

    bool getKeyState(int key) const { return window != nullptr && glfwGetKey(window, key) == GLFW_PRESS; }

    void setWindow(GLFWwindow *newWindow) { window = newWindow; }

    KeyMappings keys{};

  private:
    GLFWwindow *window{};
  };
}