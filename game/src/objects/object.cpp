#include "game/objects/objects.hpp"

namespace opengame
{
    Object::Object()
        : id(0), objpath(""), type(""), position(0.0f), rotation(0.0f), scale(1.0f), activeState(true)
    {
    }

    Object::~Object()
    {
        // Cleanup if necessary
    }

    void Object::update(float deltaTime)
    {
        // Update logic for the object can be implemented here
        // For example, you might want to update the position based on velocity, etc.
    }
}