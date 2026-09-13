#pragma once

#include "game/objects/object_manager.hpp"

namespace opengame
{
    class Scene
    {
    public:
        Scene();
        ~Scene();

        void initialize();
        void update(float deltaTime);
        void shutdown();

        ObjectManager &getObjectManager();

    private:
        ObjectManager objectManager;
    };
}