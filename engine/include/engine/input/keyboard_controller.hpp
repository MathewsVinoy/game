#pragma once

#include "engine/core/window.hpp"

#include <glm/glm.hpp>

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
    static const int kMaxKeys = 512;

    bool getKeyState(int key) const { return window != nullptr && glfwGetKey(window, key) == GLFW_PRESS; }
    bool isDown(int key) const { return key >= 0 && key < kMaxKeys && down_[key]; };
    bool wasPressed(int key) const { return key >= 0 && key < kMaxKeys && pressed_[key]; }
    glm::vec3 getMovementVector() const;

    void onKey(int key, int action);
    void setWindow(GLFWwindow *newWindow) { window = newWindow; }

    KeyMappings keys{};

  private:
    GLFWwindow *window{};
    bool down_[kMaxKeys]{false};
    bool pressed_[kMaxKeys]{false};
  };
}