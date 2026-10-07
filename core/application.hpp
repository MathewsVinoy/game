#pragma once

#include "graphics/descriptors.hpp"
#include "window.hpp"
#include "graphics/core/devices.hpp"
#include "graphics/render/object.hpp"
#include "graphics/render/camera.hpp"
#include "graphics/render/renderer.hpp"
#include "graphics/animation/animated_model.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace graphics
{

  class Application
  {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    Application();
    ~Application();

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    using UpdateCallback = std::function<void(float)>;

    void setUpdateCallback(UpdateCallback callback);
    void run();
    GameObject::id_t renderGameObjects(std::string modelPath,
                                       glm::vec3 translation = {0.f, 0.f, 0.f},
                                       glm::vec3 scale = {1.f, 1.f, 1.f},
                                       glm::vec3 rotation = {0.f, 0.f, 0.f});

    Window &getWindow();
    GameObject::Map &getGameObjects();
    Camera &getCamera();

    float getYaw() const { return yaw; }

  private:
    void loadGameObjects();

    float yaw = 0.f;

    Window window{WIDTH, HEIGHT, "Engine"};
    EngineDevice engineDevice{window};
    Renderer renderer{window, engineDevice};
    Camera camera{};

    std::unique_ptr<DescriptorPool> globalPool{};
    GameObject::Map gameObjects;
    UpdateCallback updateCallback{};
    std::shared_ptr<AnimatedModel> animatedCharacter;
  };
}