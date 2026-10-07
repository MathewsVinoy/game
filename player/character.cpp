#include "character.hpp"

#include "core/application.hpp"
#include "inputs/mouse_controller.hpp"

#include <cmath>
#include <limits>

namespace opengame
{
    Character::Character() {}

    Character::~Character() {}

    Object Character::getObject()
    {
        object.setType("Character");
        object.setPath("assets/models/char.fbx");
        object.setScale({0.01f, 0.01f, 0.01f});
        object.setPosition({0.f, 0.f, 0.f});

        return object;
    }

    void Character::loadCharacterModel(graphics::Application &app)
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

        glm::vec3 pos = object.getPosition();

        pos.y =
            GROUND_LEVEL +
            (object.getScale().y / 2.0f);

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

        auto &gameObjects =
            application->getGameObjects();

        auto it =
            gameObjects.find(renderObjectId);

        if (it == gameObjects.end() ||
            !it->second.animatedModel)
        {
            return;
        }

        auto *animator =
            it->second.animatedModel->getAnimator();

        if (isGrounded &&
            controller.getKeyState(
                keyMappings.spacebar))
        {
            verticalVelocity =
                JUMP_STRENGTH;

            isGrounded = false;

            if (!animator->isPlaying("jump"))
            {
                animator->play("jump");
            }
        }

        verticalVelocity +=
            GRAVITY_STRENGTH * dt;

        float yawRadians =
            glm::radians(cameraYaw);

        const glm::vec3 forwardDir{
            -std::sin(yawRadians),
            0.0f,
            -std::cos(yawRadians)};

        const glm::vec3 rightDir{
            std::cos(yawRadians),
            0.0f,
            -std::sin(yawRadians)};

        glm::vec3 moveDir{0.0f};

        if (controller.getKeyState(
                keyMappings.characterMoveForward))
        {
            moveDir += forwardDir;
        }

        if (controller.getKeyState(
                keyMappings.characterMoveBackward))
        {
            moveDir -= forwardDir;
        }

        if (controller.getKeyState(
                keyMappings.characterMoveRight))
        {
            moveDir += rightDir;
        }

        if (controller.getKeyState(
                keyMappings.characterMoveLeft))
        {
            moveDir -= rightDir;
        }

        float currentSpeed =
            moveSpeed;

        if (controller.getKeyState(
                keyMappings.lshift))
        {
            currentSpeed =
                sprintSpeed;
        }

        bool isMoving = false;

        if (glm::dot(moveDir, moveDir) >
            std::numeric_limits<float>::epsilon())
        {

            moveDir =
                glm::normalize(moveDir);

            object.translate(
                currentSpeed *
                dt *
                moveDir);

            float movementYaw =
                std::atan2(
                    moveDir.x,
                    moveDir.z);

            object.setRotation(
                {0.0f,
                 movementYaw,
                 0.0f});

            isMoving = true;
        }

        glm::vec3 pos =
            object.getPosition();

        pos.y +=
            verticalVelocity * dt;

        object.setPosition(pos);

        resolveGroundCollision();

        if (!isGrounded)
        {

            if (!animator->isPlaying("jump"))
            {
                animator->play("jump");
            }
        }
        else
        {
            if (isMoving)
            {
                if (currentSpeed == sprintSpeed)
                {
                    if (!animator->isPlaying("run"))
                    {
                        animator->play("run");
                    }
                }
                else
                {
                    if (!animator->isPlaying("walk"))
                    {
                        animator->play("walk");
                    }
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

        it->second.transform.translation =
            object.getPosition();

        it->second.transform.rotation =
            object.getRotation();

        it->second.transform.scale =
            object.getScale();

        updateCamera(dt);
    }

    void Character::resolveGroundCollision()
    {
        glm::vec3 pos =
            object.getPosition();

        const float halfHeight =
            object.getScale().y / 2.0f;

        const float playerBottom =
            pos.y - halfHeight;

        if (verticalVelocity <= 0.0f &&
            playerBottom <= GROUND_LEVEL)
        {
            pos.y =
                GROUND_LEVEL +
                halfHeight;

            object.setPosition(pos);

            verticalVelocity =
                0.0f;

            isGrounded =
                true;

            return;
        }

        isGrounded =
            false;
    }

    void Character::updateCamera(float dt)
    {
        if (application == nullptr)
        {
            return;
        }

        glm::vec2 mouse =
            graphics::Mouse::getMouseOffset();

        cameraYaw -=
            mouse.x *
            mouseSensitivity;

        cameraPitch -=
            mouse.y *
            mouseSensitivity;

        cameraPitch =
            glm::clamp(
                cameraPitch,
                -60.0f,
                60.0f);

        float yawRadians =
            glm::radians(cameraYaw);

        float pitchRadians =
            glm::radians(cameraPitch);

        glm::vec3 characterPosition =
            object.getPosition();

        glm::vec3 cameraTrackPos =
            characterPosition;

        cameraTrackPos.y =
            GROUND_LEVEL +
            (object.getScale().y / 2.0f);

        glm::vec3 cameraOffset;

        cameraOffset.x =
            std::sin(yawRadians) *
            std::cos(pitchRadians);

        cameraOffset.y =
            std::sin(pitchRadians);

        cameraOffset.z =
            std::cos(yawRadians) *
            std::cos(pitchRadians);

        glm::vec3 cameraPosition =
            cameraTrackPos +
            cameraOffset *
                cameraDistance;

        cameraPosition.y +=
            cameraHeight;

        if (cameraPosition.y <
            GROUND_LEVEL + 0.2f)
        {
            cameraPosition.y =
                GROUND_LEVEL + 0.2f;
        }

        glm::vec3 cameraTarget =
            cameraTrackPos +
            glm::vec3{
                0.0f,
                1.0f,
                0.0f};

        application->getCamera().setViewTarget(
            cameraPosition,
            cameraTarget,
            glm::vec3{
                0.0f,
                1.0f,
                0.0f});
    }

} // namespace opengame
