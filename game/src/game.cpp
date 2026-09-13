#include "game/game.hpp"

#include <chrono>

namespace opengame
{
    Game::Game() {}

    Game::~Game() {}

    void Game::initialize(engine::Application &app)
    {
        character.loadCharacterModel(app);
        currentTime = std::chrono::high_resolution_clock::now();
    }

    void Game::update(float deltaTime)
    {
        character.move(deltaTime);
    }

    void Game::shutdown()
    {
        // Clean up game-specific resources here
    }
}