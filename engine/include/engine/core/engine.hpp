#pragma once

#include "engine/core/application.hpp"

#include <memory>

namespace engine
{
    class Application;

    class Engine
    {
    public:
        Engine();
        ~Engine();

        void initialize();
        void run();
        void shutdown();
        Application &getApplication();

    private:
        std::unique_ptr<Application> app;
    };
}