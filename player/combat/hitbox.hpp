#pragma once

#include <glm/glm.hpp>

namespace opengame
{

    struct hitbox
    {
        glm::vec3 position{0.0f};
        glm::vec3 size{1.0f, 1.5f, 1.0f};
        bool active = false;

        glm::vec3 getMin() const { return position - size * 0.5f; }

        glm::vec3 getMax() const { return position + size * 0.5f; }
    };

    class Hitbox
    {
    public:
        bool checkAABB(const hitbox &a, const hitbox &b);
    };
}