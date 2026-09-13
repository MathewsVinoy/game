#include "game/world/scene.hpp"

namespace opengame
{
    Scene::Scene() {}

    Scene::~Scene() {}

    void Scene::initialize()
    {
        objectManager.initialize();
    }

    void Scene::update(float deltaTime)
    {
        objectManager.update(deltaTime);
    }

    void Scene::shutdown()
    {
        objectManager.shutdown();
    }

    ObjectManager &Scene::getObjectManager()
    {
        return objectManager;
    }
}