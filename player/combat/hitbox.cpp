#include "hitbox.hpp"

namespace opengame
{
    bool checkAABB(const Hitbox &a, const Hitbox &b)
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
}