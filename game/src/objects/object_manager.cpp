#include "game/objects/object_manager.hpp"
#include <algorithm>

namespace opengame
{
    ObjectManager::ObjectManager() {}

    ObjectManager::~ObjectManager() {}

    void ObjectManager::initialize()
    {
        // Initialize object manager resources here
    }

    void ObjectManager::update(float deltaTime)
    {
        for (auto &obj : objects)
        {
            if (obj.isActive())
            {
                obj.update(deltaTime);
            }
        }
    }

    void ObjectManager::shutdown()
    {
        // Clean up object manager resources here
    }

    ObjectID ObjectManager::createObject(Object config)
    {
        config.setID(nextObjectID++);
        objects.push_back(config);
        return config.getID();
    }

    Object ObjectManager::getObject(const ObjectID id)
    {
        auto it = std::find_if(objects.begin(), objects.end(), [&](const Object &obj)
                               { return obj.getID() == id; });
        if (it != objects.end())
        {
            return *it;
        }

        return Object{};
    }

    void ObjectManager::removeObject(const ObjectID id)
    {
        auto it = std::remove_if(objects.begin(), objects.end(), [&](const Object &obj)
                                 { return obj.getID() == id; });
        objects.erase(it, objects.end());
    }

    bool ObjectManager::hasObject(const ObjectID id) const
    {
        return std::any_of(objects.begin(), objects.end(), [&](const Object &obj)
                           { return obj.getID() == id; });
    }

}