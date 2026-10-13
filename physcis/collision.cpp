#include "collision.hpp"

#include <iostream>

namespace physics
{
    bool Collision::resolveGroundCollision(graphics::TransformComponent &object, float &verticalVelocity)
    {
        const float halfHeight = object.scale.y / 2.0f;
        const float playerBottom = object.translation.y - halfHeight;
        if (verticalVelocity <= 0.0f && playerBottom <= GROUND_LEVEL)
        {
            object.translation.y = GROUND_LEVEL + halfHeight;
            verticalVelocity = 0.0f;
            return true;
        }
        return false;
    }

    AABB Collision::createAABB(glm::vec3 translation, glm::vec3 scale)
    {
        AABB aabb;
        aabb.min = translation - (scale / 2.0f);
        aabb.max = translation + (scale / 2.0f);
        return aabb;
    }

    void Collision::checkCollisionAABB(const graphics::GameObject::Map &gameObjects, graphics::GameObject::id_t playerId, graphics::GameObject &player)
    {

        for (const auto &entry : gameObjects)
        {

            if (entry.first != playerId && entry.second.transform.name == "Cube")
            {

                if (checkAABB(player.transform.aabb, entry.second.transform.aabb))
                {
                    std::cout << "Collision detected between player and object with ID: " << entry.first << std::endl;
                }
            }
        }
    }

    bool Collision::checkAABB(const AABB &a, const AABB &b)
    {

        return a.min.x < b.max.x && a.max.x > b.min.x &&
               a.min.y < b.max.y && a.max.y > b.min.y &&
               a.min.z < b.max.z && a.max.z > b.min.z;
    }
}