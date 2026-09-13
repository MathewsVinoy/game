#include "game/physics/collision.hpp"

namespace opengame

{
    Collision::Collision(const glm::vec3 &playerMin, const glm::vec3 &playerMax, const glm::vec3 &groundMin, const glm::vec3 &groundMax)
        : playerMin(playerMin), playerMax(playerMax), groundMin(groundMin), groundMax(groundMax) {}

    Collision::~Collision() {}

    bool Collision::checkCollision()
    {
        bool isColliding =
            (playerMin.x <= groundMax.x) && (playerMax.x >= groundMin.x) &&
            (playerMin.y <= groundMax.y) && (playerMax.y >= groundMin.y) &&
            (playerMin.z <= groundMax.z) && (playerMax.z >= groundMin.z);
        return isColliding;
    }
}
