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
        CombatState getState() const { return state; }
        bool isAttacking() const { return state == CombatState::ATTACKING; }

    private:
        int health = 100;
        int attackDamage = 10;
        CombatState state{CombatState::IDLE};
    };

}