#include "engine/animation/skeleton.hpp"

namespace engine
{

    int Skeleton::addBone(
        const std::string &name,
        int parentIndex,
        const glm::mat4 &offsetMatrix)
    {
        Bone bone;

        bone.name = name;
        bone.index = static_cast<int>(bones.size());
        bone.parentIndex = parentIndex;
        bone.offsetMatrix = offsetMatrix;

        bones.push_back(bone);

        return static_cast<int>(bones.size() - 1);
    }

    Bone *Skeleton::findBone(const std::string &name)
    {

        for (auto &bone : bones)
        {
            if (bone.name == name)
            {
                return &bone;
            }
        }

        return nullptr;
    }

    const Bone *Skeleton::findBone(const std::string &name) const
    {

        for (const auto &bone : bones)
        {
            if (bone.name == name)
            {
                return &bone;
            }
        }

        return nullptr;
    }

    std::vector<Bone> &Skeleton::getBones()
    {
        return bones;
    }

    const std::vector<Bone> &Skeleton::getBones() const
    {
        return bones;
    }

}