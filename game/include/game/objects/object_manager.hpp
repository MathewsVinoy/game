#pragma once

#include "game/objects/objects.hpp"

#include <vector>

namespace opengame
{
    class ObjectManager
    {
    public:
        ObjectManager();
        ~ObjectManager();

        void initialize();
        void update(float deltaTime);
        void shutdown();

        ObjectID createObject(Object object);
        Object getObject(const ObjectID id);
        void removeObject(const ObjectID id);
        bool hasObject(const ObjectID id) const;

    private:
        std::vector<Object> objects;

        ObjectID nextObjectID = 1;
    };
}