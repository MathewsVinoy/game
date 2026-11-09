
#include "npc.hpp"

namespace opengame
{
    NPC::NPC() {}

    NPC::~NPC() {}

    void NPC::initNpc(graphics::Application &application)
    {
        app = &application;
    }

    void NPC::addNPC(
        std::string name,
        graphics::TransformComponent details,
        float x)
    {
        details.translation = {x, 0.f, -50.f};
        details.scale = {0.01f, 0.01f, 0.01f};

        NPC_Objectc npc_object;

        npc_object.start_point = -50.f;
        npc_object.end_point = 50.f;
        npc_object.direction = 1.0f;

        graphics::GameObject::id_t renderObjectId =
            app->loadAnimatedModel(
                "assets/models/char.fbx",
                details);

        npc_object.id = renderObjectId;
        npc_object.name = name;

        npc_objects.push_back(npc_object);
    }

    void NPC::update(float deltaTime)
    {
        if (!app)
            return;

        const float movementSpeed = 3.0f;
        const float animationSpeed = 0.3f;

        for (auto &npc_object : npc_objects)
        {
            auto it = app->getGameObjects().find(npc_object.id);

            if (it == app->getGameObjects().end())
                continue;

            graphics::GameObject &gameObject = it->second;

            if (!gameObject.animatedModel)
                continue;

            // Update animation
            gameObject.animatedModel->update(
                deltaTime * animationSpeed);

            // Move NPC
            gameObject.transform.translation.z +=
                npc_object.direction * movementSpeed * deltaTime;

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
    }

} // namespace opengame
