#pragma once

#include "npc.hpp"

namespace graphics
{
    class Application;
}

namespace opengame
{

    class NPC_Manager
    {
    public:
        void init();
        void update(float deltaTime);

    private:
        graphics::Application *app{nullptr};
    };

}