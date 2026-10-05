#pragma once

#include "graphics/render/model_buffers.hpp"
#include "skeleton.hpp"
#include "animation_clip.hpp"
#include "animator.hpp"

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