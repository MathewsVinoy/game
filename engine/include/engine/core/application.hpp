#pragma once

#include "engine/core/window.hpp"
#include "engine/render/devices.hpp"
#include "engine/render/object.hpp"
#include "engine/render/renderer.hpp"
#include "engine/core/descriptors.hpp"

#include <functional>
#include <memory>
#include <vector>
#include <string>

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
        GameObject::id_t renderGameObjects(std::string modelPath,
                                           glm::vec3 translation = {0.f, 0.f, 0.f},
                                           glm::vec3 scale = {1.f, 1.f, 1.f},
                                           glm::vec3 rotation = {0.f, 0.f, 0.f});

        Window &getWindow();
        GameObject::Map &getGameObjects();

    private:
        void loadGameObjects();

        Window window{WIDTH, HEIGHT, "Engine"};
        EngineDevice engineDevice{window};
        Renderer renderer{window, engineDevice};

        std::unique_ptr<DescriptorPool> globalPool{};
        GameObject::Map gameObjects;
        UpdateCallback updateCallback{};
    };
}