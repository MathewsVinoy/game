#include "collision.hpp"

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

    AABB Collision::createAABB(const graphics::TransformComponent &transform)
    {
        glm::vec3 extents = transform.colliderExtents;
        glm::vec3 offset = transform.colliderOffset;

        if (extents == glm::vec3(0.0f))
        {
            if (transform.name == "Player" || transform.name == "NPC")
            {
                extents = glm::vec3(0.4f, 0.9f, 0.4f);
                offset = glm::vec3(0.0f, 0.9f, 0.0f);
            }
            else
            {
                extents = transform.scale;
            }
        }

        glm::vec3 center = transform.translation + offset;
        AABB aabb;
        aabb.min = center - extents;
        aabb.max = center + extents;
        return aabb;
    }

    void Collision::checkCollisionAABB(const graphics::GameObject::Map &gameObjects,
                                       graphics::GameObject::id_t playerId,
                                       graphics::TransformComponent &player)
    {

        player.aabb = createAABB(player);

        for (const auto &entry : gameObjects)
        {
            if (entry.first == playerId)
                continue;

            const auto &objTransform = entry.second.transform;

            if (!objTransform.hasCollision && objTransform.name != "Cube" && objTransform.name != "NPC" && objTransform.name != "Player")
                continue;

            const AABB objAABB = createAABB(objTransform);

            if (!checkAABB(player.aabb, objAABB))
                continue;

            glm::vec3 overlap = calculateOverlap(player.aabb, objAABB);

            glm::vec3 playerCenter = (player.aabb.min + player.aabb.max) * 0.5f;
            glm::vec3 objCenter = (objAABB.min + objAABB.max) * 0.5f;

            if (overlap.x < overlap.y && overlap.x < overlap.z)
            {
                player.translation.x += (playerCenter.x < objCenter.x)
                                            ? -overlap.x
                                            : overlap.x;
            }
            else if (overlap.y < overlap.z)
            {
                player.translation.y += (playerCenter.y < objCenter.y)
                                            ? -overlap.y
                                            : overlap.y;
            }
            else
            {
                player.translation.z += (playerCenter.z < objCenter.z)
                                            ? -overlap.z
                                            : overlap.z;
            }

            player.aabb = createAABB(player);
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