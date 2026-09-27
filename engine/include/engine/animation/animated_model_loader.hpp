#pragma once

#include "engine/animation/skeleton.hpp"
#include "engine/animation/animation_clip.hpp"

#include <string>
#include <vector>

namespace engine
{

    class AnimatedModelLoader
    {
    public:
        static bool load(
            const std::string &filepath,
            Skeleton &skeleton,
            std::vector<AnimationClip> &animations);

    private:
        static void processNode(
            void *node,
            void *scene,
            Skeleton &skeleton,
            int parentIndex);
    };

}