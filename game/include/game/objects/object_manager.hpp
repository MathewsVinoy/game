#pragma once

namespace opengame
{
    class ObjectManager
    {
    public:
        ObjectManager();
        ~ObjectManager();

        void initialize();
        void update(float deltaTime);
        void shutdown();
    };
}