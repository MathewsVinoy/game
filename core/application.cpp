#include "application.hpp"

#include "graphics/system/render_system.hpp"
#include "inputs/keyboard_controller.h"
#include "inputs/mouse_controller.hpp"
#include "graphics/render/buffer.hpp"
#include "graphics/animation/bone_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <numeric>
#include <stdexcept>
#include <iostream>

namespace graphics
{

  Application::Application()
  {
    globalPool = DescriptorPool::Builder(engineDevice)
                     .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                     .addPoolSize(
                         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                         SwapChain::MAX_FRAMES_IN_FLIGHT)
                     .addPoolSize(
                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                         SwapChain::MAX_FRAMES_IN_FLIGHT)
                     .build();

    texturePool = DescriptorPool::Builder(engineDevice)
                      .setMaxSets(1000)
                      .addPoolSize(
                          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          1000)
                      .build();
    textureSetLayout = DescriptorSetLayout::Builder(engineDevice)
                           .addBinding(
                               0,
                               VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                               VK_SHADER_STAGE_FRAGMENT_BIT)
                           .build();
  }

  Application::~Application() {}

  void Application::setUpdateCallback(UpdateCallback callback)
  {
    updateCallback = std::move(callback);
  }

  void Application::run()
  {
    std::vector<std::unique_ptr<EngineBuffer>> uboBuffers(
        SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++)
    {
      uboBuffers[i] = std::make_unique<EngineBuffer>(
          engineDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      uboBuffers[i]->map();
    }

    std::vector<std::unique_ptr<BoneBuffer>> boneBuffers(
        SwapChain::MAX_FRAMES_IN_FLIGHT);

    std::vector<glm::mat4> initialBones(
        BoneBuffer::MAX_BONES,
        glm::mat4{1.0f});

    for (int i = 0; i < boneBuffers.size(); i++)
    {
      boneBuffers[i] =
          std::make_unique<BoneBuffer>(
              engineDevice,
              initialBones);
    }

    auto globalSetLayout =
        DescriptorSetLayout::Builder(engineDevice)
            .addBinding(
                0,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .addBinding(
                1,
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                VK_SHADER_STAGE_VERTEX_BIT)
            .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(
        SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < globalDescriptorSets.size(); i++)
    {
      auto bufferInfo = uboBuffers[i]->descriptorInfo();
      auto boneBufferInfo = boneBuffers[i]->descriptorInfo();

      DescriptorWriter(*globalSetLayout, *globalPool)
          .writeBuffer(0, &bufferInfo)
          .writeBuffer(1, &boneBufferInfo)
          .build(globalDescriptorSets[i]);
    }

    RenderSystem renderSystem{
        engineDevice,
        renderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout(),
        textureSetLayout->getDescriptorSetLayout()};

    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!window.shouldClose())
    {
      glfwPollEvents();

      auto newTime = std::chrono::high_resolution_clock::now();
      float frameTime =
          std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                     currentTime)
              .count();
      currentTime = newTime;

      Mouse::update(window.getGLFWwindow());
      if (updateCallback)
      {
        updateCallback(frameTime);
      }

      for (auto &[id, gameObject] : gameObjects)
      {
        if (gameObject.animatedModel)
        {
          gameObject.animatedModel->update(frameTime);
        }
      }

      glm::vec3 focusPoint{0.f};
      for (auto &entry : gameObjects)
      {
        if (entry.second.modelBuffer != nullptr)
        {
          focusPoint = entry.second.transform.translation;
          break;
        }
      }

      float aspect = renderer.getAspectRatio();
      camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

      if (auto commandBuffer = renderer.beginFrame())
      {
        int frameIndex = renderer.getFrameIndex();
        FrameInfo frameInfo{frameIndex,
                            frameTime,
                            commandBuffer,
                            camera,
                            globalDescriptorSets[frameIndex],
                            gameObjects};

        if (!boneBuffers.empty())
        {
          for (auto &[id, gameObject] : gameObjects)
          {
            if (gameObject.animatedModel)
            {
              auto *animator = gameObject.animatedModel->getAnimator();

              if (animator)
              {
                boneBuffers[frameIndex]->update(
                    animator->getBoneMatrices());
                break;
              }
            }
          }
        }

        GlobalUbo ubo{};
        ubo.projection = camera.getProjection();
        ubo.view = camera.getView();
        ubo.inverseView = camera.getInverseView();

        const glm::vec3 sunlightDirection =
            glm::normalize(glm::vec3(0.5f, 1.0f, 0.5f));
        ubo.sunlight.direction = glm::vec4(sunlightDirection, 0.0f);
        ubo.sunlight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.5f);
        ubo.numLights = 0;

        uboBuffers[frameIndex]->writeToBuffer(&ubo);
        uboBuffers[frameIndex]->flush();

        // render
        renderer.beginSwapChainRenderPass(commandBuffer);
        renderSystem.renderGameObjects(frameInfo);

        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
      }
    }

    vkDeviceWaitIdle(engineDevice.device());
  }

  VkDescriptorSet Application::createTextureDescriptor(Texture &texture)
  {
    VkDescriptorSet descriptorSet{};

    auto imageInfo = texture.descriptorInfo();

    DescriptorWriter(
        *textureSetLayout,
        *texturePool)
        .writeImage(0, &imageInfo)
        .build(descriptorSet);

    return descriptorSet;
  }

  Window &Application::getWindow() { return window; }

  GameObject::Map &Application::getGameObjects() { return gameObjects; }
  Camera &Application::getCamera() { return camera; }

  GameObject::id_t Application::loadStaticModel(
      const std::string &filepath,
      TransformComponent transform)
  {
    auto builder = ModelBuffer::Builder{};
    builder.loadModel(filepath);

    auto model = std::make_shared<ModelBuffer>(
        engineDevice,
        builder);

    auto object = GameObject::createGameObject();
    object.modelBuffer = model;
    object.transform = std::move(transform);
    std::cout << "Loading model: " << filepath << '\n';
    std::cout << "Texture path: "
              << builder.diffuseTexturePath << '\n';

    if (!builder.diffuseTexturePath.empty())
    {

      auto descriptorSet =
          createTextureDescriptor(*model->getTexture());

      model->setTextureDescriptorSet(descriptorSet);
    }

    const auto id = object.getId();
    gameObjects.emplace(id, std::move(object));
    std::cout << "Assigned texture descriptor to: "
              << filepath << '\n';

    return id;
  }

  GameObject::id_t Application::loadAnimatedModel(
      const std::string &filepath,
      TransformComponent transform)
  {
    auto animatedModel =
        std::make_shared<AnimatedModel>(engineDevice, filepath);

    auto object = GameObject::createGameObject();
    object.animatedModel = animatedModel;
    object.transform = std::move(transform);

    const auto id = object.getId();
    gameObjects.emplace(id, std::move(object));

    return id;
  }

}
