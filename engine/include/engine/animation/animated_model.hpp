#pragma once

#include "engine/render/model_buffers.hpp"
#include "engine/animation/skeleton.hpp"
#include "engine/animation/animation_clip.hpp"
#include "engine/animation/animator.hpp"

#include <memory>
#include <string>
#include <vector>

namespace engine
{

    class AnimatedModel
    {
    public:
        AnimatedModel(
            EngineDevice &device,
            const std::string &filepath);

        void update(float deltaTime);

        ModelBuffer *getModel() const;
        Animator *getAnimator() const;

    private:
        std::shared_ptr<ModelBuffer> model;

        Skeleton skeleton;
        std::vector<AnimationClip> animations;

        std::unique_ptr<Animator> animator;
    };

}