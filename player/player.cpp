#include "player.hpp"

#include "core/application.hpp"
#include "physcis/hitbox_collision.h"
#include "inputs/mouse_controller.hpp"

#include <cmath>
#include <limits>
#include <iostream>

namespace opengame
{
    Player::Player() {}

    Player::~Player() {}

    graphics::TransformComponent Player::getObject()
    {
        object.name = "Player";
        object.scale = {0.01f, 0.01f, 0.01f};
        object.translation = {0.f, 0.f, 0.f};
        object.rotation = {0.f, 0.f, 0.f};
        object.hasCollision = true;
        object.colliderExtents = {0.4f, 0.9f, 0.4f};
        object.colliderOffset = {0.f, 0.9f, 0.f};
        object.aabb = collision.createAABB(object);
        object.hitbox.position = object.translation + glm::vec3(0.0f, 0.9f, 0.0f);
        object.hitbox.size = glm::vec3(0.8f, 1.8f, 0.8f);
        object.hitbox.activate();

        return object;
    }

    void Player::loadPlayerModel(graphics::Application &app)
    {
        application = &app;

        controller.setWindow(app.getWindow().getGLFWwindow());

        object = getObject();

        if (object.activeState)
        {
            renderObjectId = app.loadAnimatedModel(
                "assets/models/char.fbx",
                object);
        }

        object.translation.y = GROUND_LEVEL + (object.scale.y / 2.0f);

        isGrounded = true;
        verticalVelocity = 0.0f;
    }

    void Player::move(float dt)
    {

        if (application == nullptr)

            return;

        auto &gameObjects = application->getGameObjects();

        auto it = gameObjects.find(renderObjectId);

        if (it == gameObjects.end() || !it->second.animatedModel)
            return;

        auto *animator = it->second.animatedModel->getAnimator();

        if (isGrounded && controller.getKeyState(keyMappings.spacebar))
        {
            verticalVelocity = JUMP_STRENGTH;

            isGrounded = false;

            if (!combatComponent.isAttacking() && !animator->isPlaying("jump"))
                animator->play("jump");
        }

        verticalVelocity += GRAVITY_STRENGTH * dt;

        float yawRadians = glm::radians(cameraYaw);

        const glm::vec3 forwardDir{-std::sin(yawRadians), 0.0f, -std::cos(yawRadians)};

        const glm::vec3 rightDir{std::cos(yawRadians), 0.0f, -std::sin(yawRadians)};

        glm::vec3 moveDir{0.0f};

        if (controller.getKeyState(keyMappings.playerMoveForward))
            moveDir += forwardDir;
        if (controller.getKeyState(keyMappings.playerMoveBackward))
            moveDir -= forwardDir;
        if (controller.getKeyState(keyMappings.playerMoveRight))
            moveDir += rightDir;
        if (controller.getKeyState(keyMappings.playerMoveLeft))
            moveDir -= rightDir;

        float currentSpeed = moveSpeed;

        if (controller.getKeyState(keyMappings.lshift))

            currentSpeed = sprintSpeed;

        bool isMoving = false;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        {

            moveDir = glm::normalize(moveDir);
            object.translation += currentSpeed * dt * moveDir;
            float movementYaw = std::atan2(moveDir.x, moveDir.z);
            object.rotation = {0.0f, movementYaw, 0.0f};
            isMoving = true;
        }
        std::string punchAnim = animator->hasAnimation("punching") ? "punching" : "Armature|punching";
        if (!animator->hasAnimation(punchAnim))
            punchAnim = "punch";

        bool isAttacking = false;
        glm::vec3 forward{std::sin(object.rotation.y), 0.0f, std::cos(object.rotation.y)};

        if (graphics::Mouse::leftMouseButtonPressed(application->getWindow().getGLFWwindow()))
        {
            isAttacking = true;
            combatComponent.updateState(CombatState::ATTACKING);

            if (!isMoving)
            {
                float attackYaw = std::atan2(forwardDir.x, forwardDir.z);
                object.rotation = {0.0f, attackYaw, 0.0f};
                forward = forwardDir;
            }

            if (!animator->isPlaying(punchAnim))
            {
                animator->play(punchAnim, false);
                hasDealtDamage = false;
            }

            attackhitbox.activate();
            attackhitbox.updatePosition(object.translation, forward);
        }
        else if (animator->isPlaying(punchAnim))
        {
            isAttacking = true;
            attackhitbox.activate();
            attackhitbox.updatePosition(object.translation, forward);
        }
        else
        {
            combatComponent.updateState(CombatState::IDLE);
            attackhitbox.deactivate();
            hasDealtDamage = false;
        }

        if (isAttacking && !hasDealtDamage)
        {
            for (auto &npc : application->getGameObjects())
            {
                if (npc.second.animatedModel && npc.second.transform.name == "NPC")
                {
                    if (physics::checkEnemyHit(attackhitbox, npc.second.transform.hitbox))
                    {
                        hasDealtDamage = true;
                        break;
                    }
                }
            }
        }

        object.translation.y += verticalVelocity * dt;

        isGrounded = collision.resolveGroundCollision(object, verticalVelocity);

        if (!isAttacking)
        {
            if (!isGrounded)
            {
                if (!animator->isPlaying("jump"))
                    animator->play("jump");
            }
            else
            {
                if (isMoving)
                {
                    if (currentSpeed == sprintSpeed)
                    {
                        std::string runAnim = animator->hasAnimation("Armature|running") ? "Armature|running" : "run";
                        if (!animator->isPlaying(runAnim))
                            animator->play(runAnim);
                    }
                    else
                    {
                        if (!animator->isPlaying("walk"))
                            animator->play("walk");
                    }
                }
                else
                {
                    if (!animator->isPlaying("idle"))
                        animator->play("idle");
                }
            }
        }

        collision.checkCollisionAABB(application->getGameObjects(), it->first, object);

        object.hitbox.position = object.translation + glm::vec3(0.0f, 0.9f, 0.0f);
        object.hitbox.active = true;

        it->second.transform.translation = object.translation;
        it->second.transform.hitbox = object.hitbox;

        it->second.transform.rotation = object.rotation;

        it->second.transform.scale = object.scale;
        it->second.transform.aabb = object.aabb;

        updateCamera(dt);
    }

    void Player::updateCamera(float dt)
    {
        if (application == nullptr)
            return;

        glm::vec2 mouse = graphics::Mouse::getMouseOffset();
        cameraYaw -= mouse.x * mouseSensitivity;

        cameraPitch -= mouse.y * mouseSensitivity;
        cameraPitch = glm::clamp(cameraPitch, -60.0f, 60.0f);

        float yawRadians = glm::radians(cameraYaw);
        float pitchRadians = glm::radians(cameraPitch);

        glm::vec3 playerPosition = object.translation;
        glm::vec3 cameraTrackPos = playerPosition;
        cameraTrackPos.y = GROUND_LEVEL + (object.scale.y / 2.0f);

        glm::vec3 cameraOffset;

        cameraOffset.x = std::sin(yawRadians) * std::cos(pitchRadians);
        cameraOffset.y = std::sin(pitchRadians);
        cameraOffset.z = std::cos(yawRadians) * std::cos(pitchRadians);

        glm::vec3 cameraPosition = cameraTrackPos + cameraOffset * cameraDistance;
        cameraPosition.y += cameraHeight;

        if (cameraPosition.y < GROUND_LEVEL + 0.2f)
        {
            cameraPosition.y = GROUND_LEVEL + 0.2f;
        }

        glm::vec3 cameraTarget = cameraTrackPos + glm::vec3{0.0f, 1.0f, 0.0f};

        application->getCamera().setViewTarget(cameraPosition,
                                               cameraTarget,
                                               glm::vec3{0.0f, 1.0f, 0.0f});
    }

} // namespace opengame
