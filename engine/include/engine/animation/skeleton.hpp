#pragma once

#include "engine/animation/bone.hpp"

#include <string>
#include <vector>

namespace engine
{

    class Skeleton
    {
    public:
        int addBone(
            const std::string &name,
            int parentIndex,
            const glm::mat4 &offsetMatrix);

        Bone *findBone(const std::string &name);

        const Bone *findBone(const std::string &name) const;

        std::vector<Bone> &getBones();

        const std::vector<Bone> &getBones() const;

    private:
        std::vector<Bone> bones;
    };

}