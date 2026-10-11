#pragma once

#include "player/player.hpp"
#include "graphics/core/devices.hpp"
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
        std::chrono::high_resolution_clock::time_point currentTime{};
    };
}