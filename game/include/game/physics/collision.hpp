#pragma once

#include "glm/glm.hpp"

namespace opengame
{
    class Collision
    {
    public:
        Collision() = default;
        ~Collision() = default;

        bool checkCollision(glm::vec3 position);

    private:
        float groundLevel = 0.0f;
    };
}