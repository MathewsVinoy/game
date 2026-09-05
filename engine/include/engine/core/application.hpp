#pragma once

#include "engine/core/window.hpp"
#include "engine/render/devices.hpp"
#include "engine/render/object.hpp"
#include "engine/render/renderer.hpp"
#include "engine/core/descriptors.hpp"

#include <memory>
#include <vector>

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
        void loadGameObjects();

        Window window{WIDTH, HEIGHT, "Engine"};
        EngineDevice engineDevice{window};
        Renderer renderer{window, engineDevice};

        std::unique_ptr<DescriptorPool> globalPool{};
        GameObject::Map gameObjects;
    };
}