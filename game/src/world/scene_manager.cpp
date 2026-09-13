#include "game/world/scene_manager.hpp"

namespace opengame
{
    SceneManager::SceneManager() : currentScene(nullptr) {}
    SceneManager::~SceneManager() {}

    void SceneManager::initialize() {}
    void SceneManager::update(float deltaTime) {}
    void SceneManager::shutdown() {}
    void SceneManager::loadScene(const std::string &name)
    {
        if (currentScene)
        {
            currentScene->shutdown();
        }
        currentScene = std::make_unique<Scene>();
        currentSceneName = name;
        currentScene->initialize();
    }

    Scene &SceneManager::getCurrentScene()
    {
        return *currentScene;
    }

    bool SceneManager::hasCurrentScene() const
    {
        return currentScene != nullptr;
    }

}