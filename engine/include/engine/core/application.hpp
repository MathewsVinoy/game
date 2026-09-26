#pragma once

#include "engine/core/window.hpp"
#include "engine/core/devices.hpp"
#include "engine/render/object.hpp"
#include "engine/render/renderer.hpp"
#include "engine/builder/descriptors.hpp"

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

        using UpdateCallback = std::function<void(float)>;
        void setUpdateCallback(UpdateCallback callback);

        void run();

    private:
        Window window{WIDTH, HEIGHT, "Open Game"};
        EngineDevice engineDevice{window};
        Renderer renderer{window, engineDevice};

        std::unique_ptr<DescriptorPool> globalPool{};
        GameObject::Map gameObjects;
        UpdateCallback updateCallback{};
    };
}