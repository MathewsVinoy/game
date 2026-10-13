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
        aabb.min = translation - (scale);
        aabb.max = translation + (scale);
        return aabb;
    }

    void Collision::checkCollisionAABB(const graphics::GameObject::Map &gameObjects,
                                       graphics::GameObject::id_t playerId,
                                       graphics::TransformComponent &player)
    {

        player.aabb = createAABB(player.translation, player.scale);

        for (const auto &entry : gameObjects)
        {
            if (entry.first == playerId || entry.second.transform.name != "Cube")
                continue;

            const auto &objTransform = entry.second.transform;

            const AABB objAABB = createAABB(objTransform.translation, objTransform.scale);

            if (!checkAABB(player.aabb, objAABB))
                continue;

            std::cout << "Collision detected between player and object with ID: "
                      << entry.first << std::endl;

            glm::vec3 overlap = calculateOverlap(player.aabb, objAABB);

            if (overlap.x < overlap.y && overlap.x < overlap.z)
            {
                player.translation.x += (player.translation.x < objTransform.translation.x)
                                            ? -overlap.x
                                            : overlap.x;
            }
            else if (overlap.y < overlap.z)
            {
                player.translation.y += (player.translation.y < objTransform.translation.y)
                                            ? -overlap.y
                                            : overlap.y;
            }
            else
            {
                player.translation.z += (player.translation.z < objTransform.translation.z)
                                            ? -overlap.z
                                            : overlap.z;
            }

            player.aabb = createAABB(player.translation, player.scale);
        }
    }

    bool Collision::checkAABB(const AABB &a, const AABB &b)
    {

        return a.min.x <= b.max.x && a.max.x >= b.min.x &&
               a.min.y <= b.max.y && a.max.y >= b.min.y &&
               a.min.z <= b.max.z && a.max.z >= b.min.z;
    }

    glm::vec3 Collision::calculateOverlap(const AABB &a, const AABB &b)
    {
        return glm::vec3{
            std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x),
            std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y),
            std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z)};
    }
}