#include "engine/core/engine.hpp"

namespace engine
{

    Engine::Engine() {}

    Engine::~Engine() {}

    void Engine::initialize()
    {
        app = std::make_unique<Application>();
    }

    void Engine::run()
    {
        app->run();
    }

    void Engine::shutdown()
    {
        app.reset();
    }

    Application &Engine::getApplication()
    {
        return *app;
    }

}