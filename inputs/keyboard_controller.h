#pragma once

#include "core/window.hpp"

class KeyboardMovementController
{
public:
  KeyboardMovementController() = default;
  explicit KeyboardMovementController(GLFWwindow *window) : window{window} {}

  struct KeyMappings
  {

    int playerMoveLeft = GLFW_KEY_A;
    int playerMoveRight = GLFW_KEY_D;
    int playerMoveForward = GLFW_KEY_W;
    int playerMoveBackward = GLFW_KEY_S;

    int lshift = GLFW_KEY_LEFT_SHIFT;

    int spacebar = GLFW_KEY_SPACE;
  };

  bool getKeyState(int key) const { return window != nullptr && glfwGetKey(window, key) == GLFW_PRESS; }

  void setWindow(GLFWwindow *newWindow) { window = newWindow; }

  KeyMappings keys{};

private:
  GLFWwindow *window{};
};