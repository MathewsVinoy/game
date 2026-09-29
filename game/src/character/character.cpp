#include "game/character/character.hpp"

#include "engine/core/application.hpp"

#include <cmath>
#include <limits>

namespace opengame
{
  Character::Character() {}

  Character::~Character() {}

  Object Character::getObject()
  {
    object.setType("Character");
    object.setPath("assets/models/Standard Walk.fbx");
    object.setScale({0.01f, 0.01f, 0.01f});
    object.setPosition({0.f, 0.f, 0.f});
    return object;
  }

  void Character::loadCharacterModel(engine::Application &app)
  {
    application = &app;

    controller.setWindow(app.getWindow().getGLFWwindow());

    object = getObject();

    if (object.isActive())
    {
      renderObjectId =
          app.renderGameObjects(
              object.getPath(),
              object.getPosition(),
              object.getScale(),
              object.getRotation());
    }

    // Start on the ground
    glm::vec3 pos = object.getPosition();
    pos.y = GROUND_LEVEL + (object.getScale().y / 2.0f);
    object.setPosition(pos);

    isGrounded = true;
    verticalVelocity = 0.0f;
  }

  void Character::move(float dt)
  {
    if (application == nullptr)
    {
      return;
    }

    /*
     * ---------------------------------------------------------
     * JUMP
     * ---------------------------------------------------------
     *
     * Positive vertical velocity = UP
     */
    if (isGrounded &&
        controller.getKeyState(keyMappings.spacebar))
    {
      verticalVelocity = JUMP_STRENGTH;
      isGrounded = false;
    }

    /*
     * ---------------------------------------------------------
     * GRAVITY
     * ---------------------------------------------------------
     *
     * GRAVITY_STRENGTH should be negative.
     *
     * Example:
     * GRAVITY_STRENGTH = -9.81f
     */
    verticalVelocity += GRAVITY_STRENGTH * dt;

    /*
     * ---------------------------------------------------------
     * CAMERA DIRECTION
     * ---------------------------------------------------------
     */
    float yaw = application->getYaw();

    const glm::vec3 forwardDir{
        -std::sin(yaw),
        0.0f,
        -std::cos(yaw)};

    const glm::vec3 rightDir{
        -forwardDir.z,
        0.0f,
        forwardDir.x};

    /*
     * ---------------------------------------------------------
     * HORIZONTAL MOVEMENT
     * ---------------------------------------------------------
     */
    glm::vec3 moveDir{0.0f};

    if (controller.getKeyState(keyMappings.characterMoveForward))
    {
      moveDir += forwardDir;
    }

    if (controller.getKeyState(keyMappings.characterMoveBackward))
    {
      moveDir -= forwardDir;
    }

    if (controller.getKeyState(keyMappings.characterMoveRight))
    {
      moveDir += rightDir;
    }

    if (controller.getKeyState(keyMappings.characterMoveLeft))
    {
      moveDir -= rightDir;
    }

    bool isMoving = false;

    if (glm::dot(moveDir, moveDir) >
        std::numeric_limits<float>::epsilon())
    {
      moveDir = glm::normalize(moveDir);

      object.translate(moveSpeed * dt * moveDir);

      isMoving = true;
    }

    /*
     * ---------------------------------------------------------
     * ANIMATION
     * ---------------------------------------------------------
     */
    auto &gameObjects = application->getGameObjects();

    auto it = gameObjects.find(renderObjectId);

    if (it != gameObjects.end() &&
        it->second.animatedModel)
    {
      auto *animator =
          it->second.animatedModel->getAnimator();

      if (isMoving)
      {
        if (!animator->isPlaying(
                "Armature|mixamo.com|Layer0"))
        {
          animator->play(
              "Armature|mixamo.com|Layer0");
        }
      }
      else
      {
        if (!animator->isPlaying("idle"))
        {
          animator->play("idle");
        }
      }
    }

    /*
     * ---------------------------------------------------------
     * VERTICAL MOVEMENT
     * ---------------------------------------------------------
     *
     * Positive velocity -> character goes UP
     * Negative velocity -> character goes DOWN
     */
    glm::vec3 pos = object.getPosition();

    pos.y += verticalVelocity * dt;

    object.setPosition(pos);

    /*
     * ---------------------------------------------------------
     * GROUND COLLISION
     * ---------------------------------------------------------
     */
    resolveGroundCollision();

    /*
     * ---------------------------------------------------------
     * UPDATE RENDER OBJECT
     * ---------------------------------------------------------
     */
    if (it != gameObjects.end())
    {
      it->second.transform.translation =
          object.getPosition();

      it->second.transform.rotation =
          object.getRotation();

      it->second.transform.scale =
          object.getScale();
    }
  }

  void Character::resolveGroundCollision()
  {
    glm::vec3 pos = object.getPosition();

    /*
     * Treat the object's position as its CENTER.
     *
     * Therefore:
     *
     * bottom = center - half height
     */
    const float halfHeight =
        object.getScale().y / 2.0f;

    const float playerBottom =
        pos.y - halfHeight;

    /*
     * ---------------------------------------------------------
     * FALLING / GROUND COLLISION
     * ---------------------------------------------------------
     *
     * Only collide with the ground while moving downward
     * or when already touching it.
     */
    if (verticalVelocity <= 0.0f &&
        playerBottom <= GROUND_LEVEL)
    {
      /*
       * Put the bottom of the character exactly on
       * the ground.
       */
      pos.y =
          GROUND_LEVEL + halfHeight;

      object.setPosition(pos);

      /*
       * Stop downward velocity.
       */
      verticalVelocity = 0.0f;

      /*
       * Character is now standing on the ground.
       */
      isGrounded = true;

      return;
    }

    /*
     * Character is in the air.
     */
    isGrounded = false;
  }

} // namespace opengame
