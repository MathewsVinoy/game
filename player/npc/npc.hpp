#pragma once

#include "object/object.hpp"
#include "core/application.hpp"

#include <string>

namespace opengame
{
    struct NPC_Objectc
    {
        graphics::GameObject::id_t id;
        std::string name;
    };

    class NPC
    {
    public:
        NPC();
        ~NPC();

        void addNPC(std::string name, graphics::TransformComponent details, graphics::Application &app, float x);
        void update(float deltaTime);

    private:
        std::vector<NPC_Objectc> npc_objects;
    };

}
