#pragma once

#include "graphics/render/model_buffers.hpp"
#include "graphics/animation/animated_model.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <unordered_map>

namespace graphics
{

  struct TransformComponent
  {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation{};
    bool activeState{true};

    // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
    // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
    // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
    glm::mat4 mat4();

    glm::mat3 normalMatrix();
  };

  struct PointLightComponent
  {
    float lightIntensity = 1.0f;
  };

  class GameObject
  {
  public:
    using id_t = unsigned int;
    using Map = std::unordered_map<id_t, GameObject>;

    static GameObject createGameObject()
    {
      static id_t currentId = 0;
      return GameObject{currentId++};
    }

    static GameObject makePointLight(float intensity = 10.f, float radius = 0.1f,
                                     glm::vec3 color = glm::vec3(1.f));

    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;
    GameObject(GameObject &&) = default;
    GameObject &operator=(GameObject &&) = default;

    id_t getId() { return id; }

    glm::vec3 color{};
    TransformComponent transform{};

    glm::vec3 getPosition() const { return transform.translation; }
    glm::vec3 getScale() const { return transform.scale; }
    glm::vec3 getRotation() const { return transform.rotation; }
    bool isActive() const { return transform.activeState; }

    void setPosition(const glm::vec3 &pos) { transform.translation = pos; }
    void setScale(const glm::vec3 &scale) { transform.scale = scale; }
    void setRotation(const glm::vec3 &rotation) { transform.rotation = rotation; }
    void setActive(bool active) { transform.activeState = active; }

    std::shared_ptr<ModelBuffer> modelBuffer{};
    std::shared_ptr<AnimatedModel> animatedModel{};
    std::unique_ptr<PointLightComponent> pointLight = nullptr;

  private:
    GameObject(id_t objId) : id(objId) {}
    id_t id;
  };
} // namespace graphics