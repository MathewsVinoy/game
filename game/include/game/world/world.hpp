#pragma once

namespace opengame
{
    class World
    {
    public:
        World();
        ~World();

        void initialize();
        void update(float deltaTime);
        void shutdown();
    };
}