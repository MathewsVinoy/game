#pragma once

#include "glm/glm.hpp"

namespace opengame
{
    class Collision
    {
    public:
        Collision(const glm::vec3 &playerMin, const glm::vec3 &playerMax, const glm::vec3 &groundMin, const glm::vec3 &groundMax);
        ~Collision();

        bool checkCollision();

    private:
        float groundLevel = 0.0f;
        glm::vec3 playerMin;
        glm::vec3 playerMax;
        glm::vec3 groundMin;
        glm::vec3 groundMax;
    };
}