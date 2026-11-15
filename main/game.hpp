#pragma once

#include "player/player.hpp"
#include "graphics/core/devices.hpp"
#include "object/object.hpp"
#include "player/npc/npc_manager.hpp"

#include <chrono>

namespace graphics
{
    class Application;
}

namespace opengame
{
    class Game
    {
    public:
        Game();
        ~Game();

        void initialize(graphics::Application &app);
        void update(float deltaTime);
        void shutdown();

    private:
        Player player;
        NPC_Manager npcManager;
        std::chrono::high_resolution_clock::time_point currentTime{};
    };
}