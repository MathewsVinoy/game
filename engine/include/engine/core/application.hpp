#pragma once

#include "engine/core/window.hpp"
#include "engine/core/devices.hpp"
#include "engine/render/object.hpp"
#include "engine/render/renderer.hpp"
#include "engine/builder/descriptors.hpp"
#include "engine/animation/animated_model.hpp"
#include "engine/input/keyboard_controller.hpp"

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
        void loadGameObjects();

        Window window{WIDTH, HEIGHT, "Open Game"};
        EngineDevice engineDevice{window};
        Renderer renderer{window, engineDevice};

        std::unique_ptr<DescriptorPool> globalPool{};
        GameObject::Map gameObjects;
        UpdateCallback updateCallback{};
        std::shared_ptr<AnimatedModel> animatedCharacter;
        KeyboardMovementController keyboardController{};

        float mouseYaw = 0.0f;
        float mousePitch = 0.0f;
        bool mouseInitialized = false;
    };
}