#include "engine/core/engine.hpp"
#include "game/game.hpp"

#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main()
{
    try
    {
        engine::Engine engine;
        opengame::Game game;
        engine.initialize();
        game.initialize(engine.getApplication());
        engine.getApplication().setUpdateCallback([&game](float deltaTime)
                                                  { game.update(deltaTime); });
        engine.run();
        engine.shutdown();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}