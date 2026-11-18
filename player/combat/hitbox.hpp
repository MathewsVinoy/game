#pragma once

#include <glm/glm.hpp>

namespace opengame
{

    class Hitbox
    {
    public:
        glm::vec3 position{0.0f};
        glm::vec3 size{0.8f, 0.8f, 0.8f};

        bool active = false;

        glm::vec3 getMin() const { return position - size * 0.5f; }

        glm::vec3 getMax() const { return position + size * 0.5f; }

        void activate() { active = true; }

        void deactivate() { active = false; }

        void updatePosition(const glm::vec3 &playerPosition, const glm::vec3 &forward, float reach = 0.9f)
        {
            position = playerPosition + glm::vec3(0.0f, 1.0f, 0.0f) + forward * reach;
        }
    };
}