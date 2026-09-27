#include "engine/animation/animated_model_loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include <iostream>
#include <unordered_map>

namespace engine
{

    namespace
    {

        glm::mat4 toGlm(const aiMatrix4x4 &m)
        {

            glm::mat4 result;

            result[0][0] = m.a1;
            result[1][0] = m.a2;
            result[2][0] = m.a3;
            result[3][0] = m.a4;

            result[0][1] = m.b1;
            result[1][1] = m.b2;
            result[2][1] = m.b3;
            result[3][1] = m.b4;

            result[0][2] = m.c1;
            result[1][2] = m.c2;
            result[2][2] = m.c3;
            result[3][2] = m.c4;

            result[0][3] = m.d1;
            result[1][3] = m.d2;
            result[2][3] = m.d3;
            result[3][3] = m.d4;

            return result;
        }

        void collectBones(
            const aiNode *node,
            const aiScene *scene,
            Skeleton &skeleton,
            int parentIndex,
            std::unordered_map<std::string, int> &boneMap)
        {

            std::string nodeName = node->mName.C_Str();

            int currentIndex = parentIndex;

            auto existing = boneMap.find(nodeName);

            if (existing != boneMap.end())
            {

                currentIndex = existing->second;
            }
            else
            {

                bool isBone = false;

                aiMatrix4x4 offsetMatrix;

                for (unsigned int meshIndex = 0;
                     meshIndex < scene->mNumMeshes;
                     ++meshIndex)
                {

                    const aiMesh *mesh = scene->mMeshes[meshIndex];

                    for (unsigned int boneIndex = 0;
                         boneIndex < mesh->mNumBones;
                         ++boneIndex)
                    {

                        const aiBone *bone = mesh->mBones[boneIndex];

                        if (nodeName == bone->mName.C_Str())
                        {

                            isBone = true;
                            offsetMatrix = bone->mOffsetMatrix;

                            break;
                        }
                    }

                    if (isBone)
                    {
                        break;
                    }
                }

                if (isBone)
                {

                    currentIndex = skeleton.addBone(
                        nodeName,
                        parentIndex,
                        toGlm(offsetMatrix));

                    boneMap[nodeName] = currentIndex;
                }
            }

            for (unsigned int i = 0; i < node->mNumChildren; ++i)
            {

                collectBones(
                    node->mChildren[i],
                    scene,
                    skeleton,
                    currentIndex,
                    boneMap);
            }
        }

    }

    bool AnimatedModelLoader::load(
        const std::string &filepath,
        Skeleton &skeleton,
        std::vector<AnimationClip> &animations)
    {

        Assimp::Importer importer;

        const aiScene *scene = importer.ReadFile(
            filepath,
            aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_JoinIdenticalVertices |
                aiProcess_LimitBoneWeights);

        if (!scene || !scene->mRootNode)
        {

            std::cerr
                << "Failed to load animated model: "
                << importer.GetErrorString()
                << std::endl;

            return false;
        }

        // --------------------------------------------------
        // Load skeleton
        // --------------------------------------------------

        std::unordered_map<std::string, int> boneMap;

        collectBones(
            scene->mRootNode,
            scene,
            skeleton,
            -1,
            boneMap);

        // --------------------------------------------------
        // Load animations
        // --------------------------------------------------

        animations.clear();

        for (unsigned int animationIndex = 0;
             animationIndex < scene->mNumAnimations;
             ++animationIndex)
        {

            const aiAnimation *aiAnim =
                scene->mAnimations[animationIndex];

            AnimationClip animation;

            animation.name =
                aiAnim->mName.length > 0
                    ? aiAnim->mName.C_Str()
                    : "Animation_" + std::to_string(animationIndex);

            animation.duration =
                aiAnim->mDuration;

            animation.ticksPerSecond =
                aiAnim->mTicksPerSecond > 0.0
                    ? aiAnim->mTicksPerSecond
                    : 25.0;

            // ----------------------------------------------
            // Animation channels
            // ----------------------------------------------

            for (unsigned int channelIndex = 0;
                 channelIndex < aiAnim->mNumChannels;
                 ++channelIndex)
            {

                const aiNodeAnim *channel =
                    aiAnim->mChannels[channelIndex];

                BoneAnimation boneAnimation;

                boneAnimation.boneName =
                    channel->mNodeName.C_Str();

                // ------------------------------------------
                // Positions
                // ------------------------------------------

                for (unsigned int i = 0;
                     i < channel->mNumPositionKeys;
                     ++i)
                {

                    KeyPosition key;

                    key.position = glm::vec3(
                        channel->mPositionKeys[i].mValue.x,
                        channel->mPositionKeys[i].mValue.y,
                        channel->mPositionKeys[i].mValue.z);

                    key.time =
                        channel->mPositionKeys[i].mTime;

                    boneAnimation.positions.push_back(key);
                }

                // ------------------------------------------
                // Rotations
                // ------------------------------------------

                for (unsigned int i = 0;
                     i < channel->mNumRotationKeys;
                     ++i)
                {

                    KeyRotation key;

                    key.rotation = glm::quat(
                        channel->mRotationKeys[i].mValue.w,
                        channel->mRotationKeys[i].mValue.x,
                        channel->mRotationKeys[i].mValue.y,
                        channel->mRotationKeys[i].mValue.z);

                    key.time =
                        channel->mRotationKeys[i].mTime;

                    boneAnimation.rotations.push_back(key);
                }

                // ------------------------------------------
                // Scale
                // ------------------------------------------

                for (unsigned int i = 0;
                     i < channel->mNumScalingKeys;
                     ++i)
                {

                    KeyScale key;

                    key.scale = glm::vec3(
                        channel->mScalingKeys[i].mValue.x,
                        channel->mScalingKeys[i].mValue.y,
                        channel->mScalingKeys[i].mValue.z);

                    key.time =
                        channel->mScalingKeys[i].mTime;

                    boneAnimation.scales.push_back(key);
                }

                animation.channels.push_back(
                    std::move(boneAnimation));
            }

            animations.push_back(
                std::move(animation));
        }

        // --------------------------------------------------
        // Debug output
        // --------------------------------------------------

        std::cout << "\n========== ANIMATIONS ==========\n";

        for (const auto &animation : animations)
        {

            std::cout
                << "Animation: "
                << animation.name
                << "\n";

            std::cout
                << "  Duration: "
                << animation.duration
                << "\n";

            std::cout
                << "  Ticks/sec: "
                << animation.ticksPerSecond
                << "\n";

            std::cout
                << "  Channels: "
                << animation.channels.size()
                << "\n";

            for (const auto &channel :
                 animation.channels)
            {

                std::cout
                    << "    Bone: "
                    << channel.boneName
                    << " | P:"
                    << channel.positions.size()
                    << " R:"
                    << channel.rotations.size()
                    << " S:"
                    << channel.scales.size()
                    << "\n";
            }
        }

        std::cout
            << "================================\n";

        return true;
    }
}