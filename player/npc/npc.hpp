#pragma once

#include "object/object.hpp"
#include "core/application.hpp"

#include <string>

namespace graphics
{
    class Application;
}

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

        void initNpc(graphics::Application &application);

        void addNPC(std::string name, graphics::TransformComponent details, float x);
        void update(float deltaTime);

    private:
        graphics::Application *app{nullptr};
        std::vector<NPC_Objectc> npc_objects;
    };

}
