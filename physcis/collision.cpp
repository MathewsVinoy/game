#include "collision.hpp"

#include <glm/glm.hpp>

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
}