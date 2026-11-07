#pragma once

#include "player/player.hpp"
#include "graphics/core/devices.hpp"
#include "player/npc/npc.hpp"
#include "object/object.hpp"

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
        NPC npc;
        std::chrono::high_resolution_clock::time_point currentTime{};
    };
}