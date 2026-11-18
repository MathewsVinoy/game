#pragma once

#include "object/object.hpp"
#include "core/application.hpp"
#include "physcis/collision.hpp"
#include "player/combat/hitbox.hpp"

namespace graphics
{
    class Application;
}

namespace opengame
{
    struct NPC_Objectc
    {
        graphics::GameObject::id_t id;
        std::string type;
        float start_point;
        float end_point;
        float direction = 1.0f;
    };

    class NPC
    {
    public:
        NPC(graphics::Application &application, float x, NPC_Objectc object);
        ~NPC();
        std::string getType() const { return npc_object.type; }
        void simple_walk(float deltaTime);
        void normal(float dt);

    private:
        graphics::Application *app{nullptr};
        NPC_Objectc npc_object;
        Hitbox hitbox;

        physics::Collision collision;
    };

}
