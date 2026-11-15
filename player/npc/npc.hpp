#pragma once

#include "object/object.hpp"
#include "core/application.hpp"

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
        float start_point;
        float end_point;
        float direction = 1.0f;
    };

    class NPC
    {
    public:
        NPC(graphics::Application &application, float x, NPC_Objectc object);
        ~NPC();

        void simple_walk(float deltaTime);

    private:
        graphics::Application *app{nullptr};
        NPC_Objectc npc_object;
    };

}
