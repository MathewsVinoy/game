#include "game.hpp"

#include "world/ground.h"

#include <chrono>

namespace opengame
{
    Game::Game() {}

    Game::~Game() {}

    void Game::initialize(graphics::Application &app)
    {
        ground(app);
        player.loadPlayerModel(app);
        currentTime = std::chrono::high_resolution_clock::now();
    }

    void Game::update(float deltaTime)
    {
        player.move(deltaTime);
    }

    void Game::shutdown()
    {
        // Clean up game-specific resources here
    }
}