#include "game.hpp"
#include "core/application.hpp"

#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main()
{
    try
    {
        opengame::Game game;
        engine::Application app;
        game.initialize(app);
        app.setUpdateCallback([&game](float deltaTime)
                              { game.update(deltaTime); });
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}