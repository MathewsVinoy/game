#pragma once

#include "npc.hpp"

namespace opengame
{

    class NPC_Manager
    {
    public:
        void init(graphics::Application &app);
        void update(float deltaTime);

    private:
        std::vector<NPC> npcs;
    };

}