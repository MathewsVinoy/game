#include "engine/animation/animator.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <iostream>
#include <cmath>

namespace engine
{

    Animator::Animator(
        Skeleton *skeleton,
        std::vector<AnimationClip> *animations)
        : skeleton(skeleton),
          animations(animations)
    {

        boneMatrices.resize(
            skeleton->getBones().size(),
            glm::mat4(1.0f));
    }

    void Animator::play(
        const std::string &animationName)
    {

        for (auto &animation : *animations)
        {

            if (animation.name == animationName)
            {

                currentAnimation = &animation;
                currentTime = 0.0;

                std::cout
                    << "Playing animation: "
                    << animationName
                    << "\n";

                return;
            }
        }

        std::cerr
            << "Animation not found: "
            << animationName
            << "\n";
    }

    bool Animator::isPlaying(
        const std::string &animationName) const
    {

        return currentAnimation != nullptr &&
               currentAnimation->name == animationName;
    }

    const BoneAnimation *Animator::findChannel(
        const AnimationClip &animation,
        const std::string &boneName) const
    {

        for (const auto &channel :
             animation.channels)
        {

            if (channel.boneName == boneName)
            {
                return &channel;
            }
        }

        return nullptr;
    }

    glm::vec3 Animator::interpolatePosition(
        const BoneAnimation &channel,
        double time) const
    {

        if (channel.positions.empty())
        {
            return glm::vec3(0.0f);
        }

        if (channel.positions.size() == 1)
        {
            return channel.positions[0].position;
        }

        for (size_t i = 0;
             i < channel.positions.size() - 1;
             ++i)
        {

            if (time < channel.positions[i + 1].time)
            {

                double start =
                    channel.positions[i].time;

                double end =
                    channel.positions[i + 1].time;

                float factor =
                    static_cast<float>(
                        (time - start) /
                        (end - start));

                factor = std::clamp(
                    factor,
                    0.0f,
                    1.0f);

                return glm::mix(
                    channel.positions[i].position,
                    channel.positions[i + 1].position,
                    factor);
            }
        }

        return channel.positions.back().position;
    }

    glm::quat Animator::interpolateRotation(
        const BoneAnimation &channel,
        double time) const
    {

        if (channel.rotations.empty())
        {
            return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }

        if (channel.rotations.size() == 1)
        {
            return channel.rotations[0].rotation;
        }

        for (size_t i = 0;
             i < channel.rotations.size() - 1;
             ++i)
        {

            if (time < channel.rotations[i + 1].time)
            {

                double start =
                    channel.rotations[i].time;

                double end =
                    channel.rotations[i + 1].time;

                float factor =
                    static_cast<float>(
                        (time - start) /
                        (end - start));

                factor = std::clamp(
                    factor,
                    0.0f,
                    1.0f);

                return glm::normalize(
                    glm::slerp(
                        channel.rotations[i].rotation,
                        channel.rotations[i + 1].rotation,
                        factor));
            }
        }

        return channel.rotations.back().rotation;
    }

    glm::vec3 Animator::interpolateScale(
        const BoneAnimation &channel,
        double time) const
    {

        if (channel.scales.empty())
        {
            return glm::vec3(1.0f);
        }

        if (channel.scales.size() == 1)
        {
            return channel.scales[0].scale;
        }

        for (size_t i = 0;
             i < channel.scales.size() - 1;
             ++i)
        {

            if (time < channel.scales[i + 1].time)
            {

                double start =
                    channel.scales[i].time;

                double end =
                    channel.scales[i + 1].time;

                float factor =
                    static_cast<float>(
                        (time - start) /
                        (end - start));

                factor = std::clamp(
                    factor,
                    0.0f,
                    1.0f);

                return glm::mix(
                    channel.scales[i].scale,
                    channel.scales[i + 1].scale,
                    factor);
            }
        }

        return channel.scales.back().scale;
    }

    void Animator::calculateBoneTransforms(
        int boneIndex,
        const glm::mat4 &parentTransform)
    {

        auto &bone =
            skeleton->getBones()[boneIndex];

        glm::mat4 localTransform =
            bone.localTransform;

        if (currentAnimation != nullptr)
        {

            const BoneAnimation *channel =
                findChannel(
                    *currentAnimation,
                    bone.name);

            if (channel != nullptr)
            {

                glm::vec3 position =
                    interpolatePosition(
                        *channel,
                        currentTime);

                glm::quat rotation =
                    interpolateRotation(
                        *channel,
                        currentTime);

                glm::vec3 scale =
                    interpolateScale(
                        *channel,
                        currentTime);

                localTransform =
                    glm::translate(
                        glm::mat4(1.0f),
                        position) *
                    glm::mat4_cast(rotation) *
                    glm::scale(
                        glm::mat4(1.0f),
                        scale);
            }
        }

        bone.globalTransform =
            parentTransform * localTransform;

        boneMatrices[boneIndex] =
            bone.globalTransform *
            bone.offsetMatrix;

        for (size_t i = 0;
             i < skeleton->getBones().size();
             ++i)
        {

            if (
                skeleton->getBones()[i].parentIndex == boneIndex)
            {

                calculateBoneTransforms(
                    static_cast<int>(i),
                    bone.globalTransform);
            }
        }
    }

    void Animator::update(float deltaTime)
    {

        if (
            currentAnimation == nullptr ||
            currentAnimation->duration <= 0.0)
        {
            return;
        }

        currentTime +=
            deltaTime *
            currentAnimation->ticksPerSecond;

        currentTime =
            std::fmod(
                currentTime,
                currentAnimation->duration);

        for (size_t i = 0;
             i < skeleton->getBones().size();
             ++i)
        {

            if (
                skeleton->getBones()[i].parentIndex == -1)
            {

                calculateBoneTransforms(
                    static_cast<int>(i),
                    glm::mat4(1.0f));
            }
        }
    }

    const std::vector<glm::mat4> &
    Animator::getBoneMatrices() const
    {

        return boneMatrices;
    }

}