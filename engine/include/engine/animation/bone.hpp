#pragma once

#include <glm/glm.hpp>
#include <string>

namespace engine
{

    struct Bone
    {
        std::string name;

        int index = -1;
        int parentIndex = -1;

        glm::mat4 offsetMatrix{1.0f};
        glm::mat4 localTransform{1.0f};
        glm::mat4 globalTransform{1.0f};
    };

}