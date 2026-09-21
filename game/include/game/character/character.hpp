#pragma once

#include "engine/input/keyboard_controller.hpp"
#include "game/objects/objects.hpp"

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

  private:
    Object object;
    engine::KeyboardMovementController controller;
    engine::KeyboardMovementController::KeyMappings keyMappings;
    engine::Application *application{nullptr};
    engine::GameObject::id_t renderObjectId{0};

    float moveSpeed = 5.0f;
    float verticalVelocity = 0.0f;
    bool isGrounded = true;
    const float GRAVITY_STRENGTH = 14.0f;
    const float JUMP_STRENGTH = -6.0f;
    const float GROUND_LEVEL = 0.0f;
  };
} // namespace opengame