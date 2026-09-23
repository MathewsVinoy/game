#pragma once

#include "engine/core/window.hpp"
#include "engine/core/devices.hpp"

namespace engine
{
    class Application
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        Application();
        ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        void run();

    private:
        Window window{WIDTH, HEIGHT, "Open Game"};
        EngineDevice engineDevice{window};
    };
}