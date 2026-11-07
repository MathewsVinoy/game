#include "npc.hpp"

namespace opengame

{
    NPC::NPC() {}
    NPC::~NPC() {}

    void NPC::addNPC(std::string name, graphics::TransformComponent details, graphics::Application &app, float x)
    {

        details.translation = {x, 0.f, 0.f};
        details.scale = {0.01f, 0.01f, 0.01f};

        NPC_Objectc npc_object;

        graphics::GameObject::id_t renderObjectId = app.loadAnimatedModel(
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
            npc_object.details.translation.x += 0.01f * deltaTime;
        }
    }

} // namespace opengame
