#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>
#include <iostream>

namespace graphics
{
  struct SimplePushConstantData
  {
    glm::mat4 modelMatrix{1.f};
    glm::mat4 normalMatrix{1.f};
    int uSkinned{0};
    int hasTexture{0};
  };

  RenderSystem::RenderSystem(
      EngineDevice &engineDevice,
      VkRenderPass renderPass,
      VkDescriptorSetLayout globalSetLayout,
      VkDescriptorSetLayout boneSetLayout,
      VkDescriptorSetLayout textureSetLayout,
      VkDescriptorSet defaultTextureSet,
      VkDescriptorSet defaultBoneSet)
      : engineDevice{engineDevice},
        defaultTextureSet{defaultTextureSet},
        defaultBoneSet{defaultBoneSet}
  {
    createPipelineLayout(
        globalSetLayout,
        boneSetLayout,
        textureSetLayout);

    createPipeline(renderPass);
  }

  RenderSystem::~RenderSystem()
  {
    vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
  }

  void RenderSystem::createPipelineLayout(
      VkDescriptorSetLayout globalSetLayout,
      VkDescriptorSetLayout boneSetLayout,
      VkDescriptorSetLayout textureSetLayout)
  {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
        globalSetLayout,
        boneSetLayout,
        textureSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount =
        static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(
            engineDevice.device(),
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create pipeline layout!");
    }
  }

  void RenderSystem::createPipeline(VkRenderPass renderPass)
  {
    assert(pipelineLayout != nullptr &&
           "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(
        engineDevice, "build/shaders/triangle.vert.spv",
        "build/shaders/triangle.frag.spv", pipelineConfig);
  }

  void RenderSystem::renderGameObjects(FrameInfo &frameInfo)
  {
    pipeline->bind(frameInfo.commandBuffer);

    // Set 0: global camera and lighting data.
    vkCmdBindDescriptorSets(
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &frameInfo.globalDescriptorSet,
        0,
        nullptr);

    // Set 1: default identity bone buffer
    if (defaultBoneSet != VK_NULL_HANDLE)
    {
      vkCmdBindDescriptorSets(
          frameInfo.commandBuffer,
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          pipelineLayout,
          1,
          1,
          &defaultBoneSet,
          0,
          nullptr);
    }

    // Set 2: default fallback texture
    if (defaultTextureSet != VK_NULL_HANDLE)
    {
      vkCmdBindDescriptorSets(
          frameInfo.commandBuffer,
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          pipelineLayout,
          2,
          1,
          &defaultTextureSet,
          0,
          nullptr);
    }

    for (auto &kv : frameInfo.gameObjects)
    {
      auto &obj = kv.second;

      if (obj.modelBuffer == nullptr &&
          obj.animatedModel == nullptr)
      {
        continue;
      }

      SimplePushConstantData push{};
      push.modelMatrix = obj.transform.mat4();
      push.normalMatrix = glm::mat4(
          glm::transpose(
              glm::inverse(glm::mat3(obj.transform.mat4()))));

      push.uSkinned = (obj.animatedModel != nullptr) ? 1 : 0;
      push.hasTexture = 0;

      if (obj.animatedModel)
      {
        // Set 1: this character's bone matrices.
        const auto frameIndex =
            static_cast<size_t>(frameInfo.frameIndex);

        if (frameIndex >= obj.boneDescriptorSets.size() ||
            obj.boneDescriptorSets[frameIndex] == VK_NULL_HANDLE)
        {
          throw std::runtime_error(
              "Animated object has no bone descriptor set");
        }

        VkDescriptorSet boneSet =
            obj.boneDescriptorSets[frameIndex];

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            1,
            1,
            &boneSet,
            0,
            nullptr);

        if (defaultTextureSet != VK_NULL_HANDLE)
        {
          vkCmdBindDescriptorSets(
              frameInfo.commandBuffer,
              VK_PIPELINE_BIND_POINT_GRAPHICS,
              pipelineLayout,
              2,
              1,
              &defaultTextureSet,
              0,
              nullptr);
        }

        obj.animatedModel->getModel()->bind(
            frameInfo.commandBuffer);

        vkCmdPushConstants(
            frameInfo.commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT |
                VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);

        obj.animatedModel->getModel()->draw(
            frameInfo.commandBuffer);
      }
      else
      {
        if (defaultBoneSet != VK_NULL_HANDLE)
        {
          vkCmdBindDescriptorSets(
              frameInfo.commandBuffer,
              VK_PIPELINE_BIND_POINT_GRAPHICS,
              pipelineLayout,
              1,
              1,
              &defaultBoneSet,
              0,
              nullptr);
        }

        obj.modelBuffer->bind(frameInfo.commandBuffer);

        // Set 2: static model texture.
        VkDescriptorSet textureSet =
            obj.modelBuffer->getTextureDescriptorSet();

        if (textureSet != VK_NULL_HANDLE)
        {
          push.hasTexture = 1;

          vkCmdBindDescriptorSets(
              frameInfo.commandBuffer,
              VK_PIPELINE_BIND_POINT_GRAPHICS,
              pipelineLayout,
              2,
              1,
              &textureSet,
              0,
              nullptr);
        }
        else
        {
          push.hasTexture = 0;
          if (defaultTextureSet != VK_NULL_HANDLE)
          {
            vkCmdBindDescriptorSets(
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                2,
                1,
                &defaultTextureSet,
                0,
                nullptr);
          }
        }

        vkCmdPushConstants(
            frameInfo.commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT |
                VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);

        obj.modelBuffer->draw(frameInfo.commandBuffer);
      }
    }
  }

} // namespace graphics