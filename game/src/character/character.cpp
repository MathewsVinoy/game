#include "game/character/character.hpp"

#include "engine/core/application.hpp"

namespace opengame {
Character::Character() {}

Character::~Character() {}

Object Character::getObject() {
  object.setType("Character");
  object.setPath("assets/models/cube.obj");
  object.setScale({0.5f, 0.5f, 0.5f});
  object.setPosition({0.f, -1.0f, 0.f});
  return object;
}

void Character::loadCharacterModel(engine::Application &app) {
  application = &app;
  controller.setWindow(app.getWindow().getGLFWwindow());
  object = getObject();
  if (object.isActive()) {
    renderObjectId =
        app.renderGameObjects(object.getPath(), object.getPosition(),
                              object.getScale(), object.getRotation());
  }
}

void Character::move(float dt) {
  if (application == nullptr) {
    return;
  }

  // 1. Jump input (before gravity so full jump impulse is applied)
  if (isGrounded && controller.getKeyState(keyMappings.spacebar)) {
    verticalVelocity = JUMP_STRENGTH;
    isGrounded = false;
  }

  // 2. Apply gravity
  verticalVelocity += GRAVITY_STRENGTH * dt;

  // 3. Horizontal movement
  float yaw = object.getRotation().y;
  const glm::vec3 forwardDir{-sin(yaw), 0.f, -cos(yaw)};
  const glm::vec3 rightDir{-forwardDir.z, 0.f, forwardDir.x};

  glm::vec3 moveDir{0.f};
  if (controller.getKeyState(keyMappings.characterMoveForward))
    moveDir += forwardDir;
  if (controller.getKeyState(keyMappings.characterMoveBackward))
    moveDir -= forwardDir;
  if (controller.getKeyState(keyMappings.characterMoveRight))
    moveDir += rightDir;
  if (controller.getKeyState(keyMappings.characterMoveLeft))
    moveDir -= rightDir;

  if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
    object.translate(moveSpeed * dt * glm::normalize(moveDir));
  }

  // 4. Apply vertical velocity
  glm::vec3 pos = object.getPosition();
  pos.y += verticalVelocity * dt;
  object.setPosition(pos);

  // 5. Resolve collision AFTER movement
  resolveGroundCollision();

  // 6. Sync render objects
  auto &gameObjects = application->getGameObjects();
  auto it = gameObjects.find(renderObjectId);
  if (it != gameObjects.end()) {
    it->second.transform.translation = object.getPosition();
    it->second.transform.rotation = object.getRotation();
    it->second.transform.scale = object.getScale();
  }
}

void Character::resolveGroundCollision() {
  // Skip collision when moving upward (negative velocity = upward in Y-down)
  // This prevents the ground from cancelling an active jump
  if (verticalVelocity < 0.0f) {
    isGrounded = false;
    return;
  }

  glm::vec3 playerMin = object.getPosition() - (object.getScale() / 2.0f);
  glm::vec3 playerMax = object.getPosition() + (object.getScale() / 2.0f);

  // Ground AABB – thin slab whose TOP face is at GROUND_LEVEL
  glm::vec3 groundMin = {-1e6f, GROUND_LEVEL - 0.1f, -1e6f};
  glm::vec3 groundMax = {1e6f, GROUND_LEVEL, 1e6f};

  // Check collision
  bool isColliding =
      (playerMin.x <= groundMax.x) && (playerMax.x >= groundMin.x) &&
      (playerMin.y <= groundMax.y) && (playerMax.y >= groundMin.y) &&
      (playerMin.z <= groundMax.z) && (playerMax.z >= groundMin.z);

  if (isColliding) {
    glm::vec3 pos = object.getPosition();
    glm::vec3 playerScale = object.getScale();
    pos.y = groundMax.y - playerScale.y / 2.0f;
    object.setPosition(pos);
    verticalVelocity = 0.0f;
    isGrounded = true;
  } else {
    isGrounded = false;
  }
}
} // namespace opengame