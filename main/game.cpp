#include "game.hpp"

#include "world/ground.h"
#include "world/road.h"
#include "world/world.h"

#include <chrono>

namespace opengame
{
    Game::Game() {}

    Game::~Game() {}

    void Game::initialize(graphics::Application &app)
    {

        world(app);
        player.loadPlayerModel(app);
        npc.addNPC(1, "Alice", graphics::TransformComponent{}, app, 5.f);
        npc.addNPC(2, "Bob", graphics::TransformComponent{}, app, -5.f);
        currentTime = std::chrono::high_resolution_clock::now();
    }

    void Game::update(float deltaTime)
    {
        player.move(deltaTime);
        npc.update(deltaTime);
    }

    void Game::shutdown()
    {
        // Clean up game-specific resources here
    }
}