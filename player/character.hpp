#pragma once

#include "inputs/keyboard_controller.h"
#include "objects.hpp"

namespace engine
{
  class Application;
}

namespace opengame
{
  class Character
  {
  public:
    Character();
    ~Character();

    Object getObject();
    void move(float dt);
    void loadCharacterModel(engine::Application &app);
    void resolveGroundCollision();
    void updateCamera(float dt);

  private:
    Object object;
    KeyboardMovementController controller;
    KeyboardMovementController::KeyMappings keyMappings;
    engine::Application *application{nullptr};
    engine::GameObject::id_t renderObjectId{0};

    float moveSpeed = 5.0f;
    float sprintSpeed = 100.0f;

    float verticalVelocity = 0.0f;
    bool isGrounded = true;
    const float GRAVITY_STRENGTH = -9.81f;
    const float JUMP_STRENGTH = 6.0f;
    const float GROUND_LEVEL = 0.0f;
    float cameraYaw{0.0f};
    float cameraPitch{15.0f};

    float mouseSensitivity{0.1f};
    float cameraDistance{8.0f};
    float cameraHeight{3.0f};
  };
} // namespace opengame