#pragma once

#include "game/world/scene.hpp"

#include <memory>
#include <string>

namespace opengame
{
    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager();

        void initialize();
        void update(float deltaTime);
        void shutdown();

        void loadScene(const std::string &name);

        Scene &getCurrentScene();

        bool hasCurrentScene() const;

    private:
        std::unique_ptr<Scene> currentScene;
        std::string currentSceneName;
    };
}
