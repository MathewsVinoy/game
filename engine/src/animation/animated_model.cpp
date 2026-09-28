#include "engine/animation/animated_model.hpp"

#include "engine/animation/animated_model_loader.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace engine
{
    namespace
    {
        std::string toLowerCopy(std::string value)
        {
            std::transform(
                value.begin(),
                value.end(),
                value.begin(),
                [](unsigned char ch)
                { return static_cast<char>(std::tolower(ch)); });
            return value;
        }
    }

    AnimatedModel::AnimatedModel(
        EngineDevice &device,
        const std::string &filepath)
    {
        if (!AnimatedModelLoader::load(
                filepath,
                skeleton,
                animations))
        {
            throw std::runtime_error(
                "Failed to load animated model: " + filepath);
        }

        ModelBuffer::Builder builder{};
        builder.loadAnimatedModel(filepath, skeleton);

        model = std::make_shared<ModelBuffer>(
            device,
            builder);

        animator = std::make_unique<Animator>(
            &skeleton,
            &animations);

        if (!animations.empty())
        {
            animator->play(findIdleAnimationName());
        }
    }

    void AnimatedModel::update(float deltaTime)
    {
        if (animator)
        {
            animator->update(deltaTime);
        }
    }

    void AnimatedModel::setMovement(bool isMoving)
    {
        if (!animator || animations.empty())
        {
            return;
        }

        const std::string preferredAnimation =
            isMoving ? findMovementAnimationName()
                     : findIdleAnimationName();

        if (!preferredAnimation.empty() &&
            !animator->isPlaying(preferredAnimation))
        {
            animator->play(preferredAnimation);
        }
    }

    std::string AnimatedModel::findIdleAnimationName() const
    {
        for (const auto &animation : animations)
        {
            const std::string lowerName = toLowerCopy(animation.name);
            if (lowerName.find("idle") != std::string::npos)
            {
                return animation.name;
            }
        }

        return animations.empty() ? "" : animations.front().name;
    }

    std::string AnimatedModel::findMovementAnimationName() const
    {
        for (const auto &animation : animations)
        {
            const std::string lowerName = toLowerCopy(animation.name);
            if (lowerName.find("walk") != std::string::npos ||
                lowerName.find("run") != std::string::npos ||
                lowerName.find("move") != std::string::npos)
            {
                return animation.name;
            }
        }

        for (const auto &animation : animations)
        {
            const std::string lowerName = toLowerCopy(animation.name);
            if (lowerName.find("layer0") != std::string::npos &&
                lowerName.find("idle") == std::string::npos)
            {
                return animation.name;
            }
        }

        return findIdleAnimationName();
    }

    ModelBuffer *AnimatedModel::getModel() const
    {
        return model.get();
    }

    Animator *AnimatedModel::getAnimator() const
    {
        return animator.get();
    }

}