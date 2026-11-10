#pragma once

namespace opengame
{
    enum class CombatState
    {
        NONE,
        IDLE,
        ATTACKING,
        BLOCKING,
        TAKING_DAMAGE,
        DEAD
    };

    class CombatComponent
    {
    public:
        CombatComponent() = default;
        ~CombatComponent() = default;

        void updateState(CombatState newState) { state = newState; }

    private:
        int health = 100;
        int attackDamage = 10;
        CombatState state;
    };

}