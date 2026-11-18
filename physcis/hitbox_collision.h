#pragma once

#include "player/combat/hitbox.hpp"

#include <iostream>

namespace physics
{

    inline bool checkHitboxAABB(const opengame::Hitbox &a, const opengame::Hitbox &b)
    {
        if (!a.active || !b.active)
            return false;
        return (
            a.getMin().x <= b.getMax().x &&
            a.getMax().x >= b.getMin().x &&
            a.getMin().y <= b.getMax().y &&
            a.getMax().y >= b.getMin().y &&
            a.getMin().z <= b.getMax().z &&
            a.getMax().z >= b.getMin().z);
    }

    inline bool checkEnemyHit(const opengame::Hitbox &player, const opengame::Hitbox &enemyHitbox)
    {
        if (checkHitboxAABB(player, enemyHitbox))
        {
            std::cout << "ENEMY HIT!" << std::endl;
            return true;
        }
        return false;
    }

}
