#pragma once

#include "object/object.hpp"

namespace physics
{
    class Collision
    {
    public:
        bool resolveGroundCollision(graphics::TransformComponent &object, float &verticalVelocity);

    private:
        const float GROUND_LEVEL = 0.0f;
    };

}