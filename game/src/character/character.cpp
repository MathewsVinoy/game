#include "game/character/character.hpp"

#include "engine/core/application.hpp"

namespace opengame
{
    Character::Character() {}

    Character::~Character() {}

    Object Character::getObject()
    {
        object.setType("Character");
        object.setPath("assets/models/cube.obj");
        object.setScale({0.5f, 0.5f, 0.5f});
        object.setPosition({0.f, -1.0f, 0.f});
        return object;
    }

    void Character::loadCharacterModel(engine::Application &app)
    {
        application = &app;
        controller.setWindow(app.getWindow().getGLFWwindow());
        object = getObject();
        if (object.isActive())
        {
            renderObjectId = app.renderGameObjects(object.getPath(), object.getPosition(), object.getScale(), object.getRotation());
        }
    }

    void Character::move(float dt)
    {
        if (application == nullptr)
        {
            return;
        }

        resolveGroundCollision();

        if (isGrounded && controller.getKeyState(keyMappings.spacebar))
        {
            verticalVelocity = JUMP_STRENGTH;
            isGrounded = false;
        }

        verticalVelocity += GRAVITY_STRENGTH * dt;

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

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        {
            object.translate(moveSpeed * dt * glm::normalize(moveDir));
        }

        glm::vec3 pos = object.getPosition();
        pos.y += verticalVelocity * dt;
        object.setPosition(pos);

        auto &gameObjects = application->getGameObjects();
        auto it = gameObjects.find(renderObjectId);
        if (it != gameObjects.end())
        {
            it->second.transform.translation = object.getPosition();
            it->second.transform.rotation = object.getRotation();
            it->second.transform.scale = object.getScale();
        }
    }

    void Character::resolveGroundCollision()
    {
        glm::vec3 playerMin = object.getPosition() - (object.getScale() / 2.0f);
        glm::vec3 playerMax = object.getPosition() + (object.getScale() / 2.0f);

        // Ground AABB
        glm::vec3 groundMin = {-1e6f, GROUND_LEVEL, -1e6f};
        glm::vec3 groundMax = {1e6f, GROUND_LEVEL - 0.1f, 1e6f};

        // Check collision
        bool isColliding =
            (playerMin.x <= groundMax.x) && (playerMax.x >= groundMin.x) &&
            (playerMin.y <= groundMax.y) && (playerMax.y >= groundMin.y) &&
            (playerMin.z <= groundMax.z) && (playerMax.z >= groundMin.z);

        if (isColliding)
        {
            glm::vec3 pos = object.getPosition();
            glm::vec3 playerScale = object.getScale();
            pos.y = groundMax.y - playerScale.y / 2.0f;
            object.setPosition(pos);
            verticalVelocity = 0.0f;
            isGrounded = true;
        }
        else
        {
            isGrounded = false;
        }
    }
}