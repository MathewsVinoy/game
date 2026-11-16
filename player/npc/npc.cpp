
#include "npc.hpp"

namespace opengame
{
    NPC::NPC(graphics::Application &application, float x, NPC_Objectc object)
    {
        app = &application;
        npc_object = object;
        graphics::TransformComponent info{};
        info.name = "NPC";
        info.translation = {x, 0.f, -50.f};
        info.scale = {0.01f, 0.01f, 0.01f};
        info.hasCollision = true;
        info.colliderExtents = {0.4f, 0.9f, 0.4f};
        info.colliderOffset = {0.f, 0.9f, 0.f};
        info.aabb = collision.createAABB(info);

        graphics::GameObject::id_t renderObjectId = app->loadAnimatedModel(
            "assets/models/char.fbx",
            info);
        npc_object.id = renderObjectId;
    }

    NPC::~NPC() {}

    void NPC::simple_walk(float deltaTime)
    {

        const float movementSpeed = 3.0f;
        const float animationSpeed = 0.3f;

        auto it = app->getGameObjects().find(npc_object.id);

        if (it == app->getGameObjects().end())
            return;

        graphics::GameObject &gameObject = it->second;

        if (!gameObject.animatedModel)
            return;

        // Update animation
        gameObject.animatedModel->update(
            deltaTime * animationSpeed);

        // Move NPC
        gameObject.transform.translation.z +=
            npc_object.direction * movementSpeed * deltaTime;
        gameObject.transform.aabb = collision.createAABB(gameObject.transform);

        // Check movement boundaries
        if (gameObject.transform.translation.z >= npc_object.end_point)
        {
            gameObject.transform.translation.z = npc_object.end_point;
            npc_object.direction = -1.0f;

            gameObject.transform.rotation.y = 3.14159f;
        }
        else if (gameObject.transform.translation.z <= npc_object.start_point)
        {
            gameObject.transform.translation.z = npc_object.start_point;
            npc_object.direction = 1.0f;

            gameObject.transform.rotation.y = 0.0f;
        }

        // Play walking animation
        auto *animator = gameObject.animatedModel->getAnimator();

        if (animator && !animator->isPlaying("walk"))
        {
            animator->play("walk");
        }
    }

    void NPC::normal(float dt)
    {
        auto it = app->getGameObjects().find(npc_object.id);
        if (it == app->getGameObjects().end())
            return;
        collision.checkCollisionAABB(app->getGameObjects(), it->first, it->second.transform);
    }

} // namespace opengame
