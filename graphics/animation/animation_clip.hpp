#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

namespace engine
{

    struct KeyPosition
    {
        glm::vec3 position;
        double time;
    };

    struct KeyRotation
    {
        glm::quat rotation;
        double time;
    };

    struct KeyScale
    {
        glm::vec3 scale;
        double time;
    };

    struct BoneAnimation
    {

        std::string boneName;

        std::vector<KeyPosition> positions;
        std::vector<KeyRotation> rotations;
        std::vector<KeyScale> scales;
    };

    class AnimationClip
    {
    public:
        std::string name;

        double duration = 0.0;
        double ticksPerSecond = 25.0;

        std::vector<BoneAnimation> channels;
    };

}