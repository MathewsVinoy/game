#include "npc.hpp"

namespace opengame

{
    NPC::NPC() {}
    NPC::~NPC() {}

    void NPC::initNpc(graphics::Application &application)
    {
        app = &application;
    }

    void NPC::addNPC(std::string name, graphics::TransformComponent details, float x)
    {

        details.translation = {x, 0.f, 0.f};
        details.scale = {0.01f, 0.01f, 0.01f};

        NPC_Objectc npc_object;

        graphics::GameObject::id_t renderObjectId = app->loadAnimatedModel(
            "assets/models/char.fbx",
            details);

        npc_object.id = renderObjectId;
        npc_object.name = name;
        npc_objects.push_back(npc_object);
    }

    void NPC::update(float deltaTime)
    {
        for (auto &npc_object : npc_objects)
        {
            auto it = app->getGameObjects().find(npc_object.id);
            if (it != app->getGameObjects().end())
            {
                graphics::GameObject &gameObject = it->second;
                if (gameObject.animatedModel)
                {
                    float animationSpeed = 0.4f; // 40% of normal speed
                    gameObject.animatedModel->update(deltaTime * animationSpeed);
                }
                gameObject.transform.translation.z += 1.0f * deltaTime;
                auto *animator = gameObject.animatedModel->getAnimator();

                if (!animator->isPlaying("walk"))
                    animator->play("walk");
            }
        }
    }

} // namespace opengame
