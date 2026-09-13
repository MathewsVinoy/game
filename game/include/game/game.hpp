#pragma once

#include "game/character/character.hpp"

#include <chrono>

namespace engine
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

        void initialize(engine::Application &app);
        void update(float deltaTime);
        void shutdown();

    private:
        Character character;
        std::chrono::high_resolution_clock::time_point currentTime{};
    };
}