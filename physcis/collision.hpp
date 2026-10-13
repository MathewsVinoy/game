#pragma once

#include "aabb.h"
#include "object/object.hpp"

namespace physics
{

    class Collision
    {
    public:
        bool resolveGroundCollision(graphics::TransformComponent &object, float &verticalVelocity);

        AABB createAABB(glm::vec3 translation, glm::vec3 scale);
        void checkCollisionAABB(const graphics::GameObject::Map &gameObjects, graphics::GameObject::id_t playerId, graphics::GameObject &player);

    private:
        bool checkAABB(const AABB &a, const AABB &b);

        const float GROUND_LEVEL = 0.0f;
    };

}