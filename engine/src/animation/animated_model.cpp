#include "engine/animation/animated_model.hpp"

#include "engine/animation/animated_model_loader.hpp"

#include <stdexcept>

namespace engine
{

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
            animator->play(animations[0].name);
        }
    }

    void AnimatedModel::update(float deltaTime)
    {
        if (animator)
        {
            animator->update(deltaTime);
        }
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