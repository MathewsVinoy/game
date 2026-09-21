#pragma once

#include "engine/animation/skeleton.hpp"
#include "engine/animation/animation_clip.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace engine
{

    class Animator
    {
    public:
        Animator(
            Skeleton *skeleton,
            std::vector<AnimationClip> *animations);

        void play(const std::string &animationName);

        void update(float deltaTime);

        const std::vector<glm::mat4> &getBoneMatrices() const;

        bool isPlaying(const std::string &animationName) const;

    private:
        Skeleton *skeleton;
        std::vector<AnimationClip> *animations;

        AnimationClip *currentAnimation = nullptr;

        double currentTime = 0.0;

        std::vector<glm::mat4> boneMatrices;

        const BoneAnimation *findChannel(
            const AnimationClip &animation,
            const std::string &boneName) const;

        glm::vec3 interpolatePosition(
            const BoneAnimation &channel,
            double time) const;

        glm::quat interpolateRotation(
            const BoneAnimation &channel,
            double time) const;

        glm::vec3 interpolateScale(
            const BoneAnimation &channel,
            double time) const;

        void calculateBoneTransforms(
            int boneIndex,
            const glm::mat4 &parentTransform);
    };

}