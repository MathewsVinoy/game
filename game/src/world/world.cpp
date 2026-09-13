#include "game/world/world.hpp"

namespace opengame
{
    World::World() {}

    World::~World() {}

    void World::initialize()
    {
        objectManager.initialize();
        sceneManager.initialize();
    }

    void World::update(float deltaTime)
    {
        objectManager.update(deltaTime);
        sceneManager.update(deltaTime);
    }

    void World::shutdown()
    {
        objectManager.shutdown();
        sceneManager.shutdown();
    }
}