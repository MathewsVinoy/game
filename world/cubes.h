#pragma once

#include "object/object.hpp"

void addCube(graphics::EngineDevice &device, glm::vec3 pos, graphics::GameObject::Map &gameObjects)
{

    std::shared_ptr<graphics::ModelBuffer> model = graphics::ModelBuffer::createModelFromFile(device, "assets/models/cube.obj");
    auto building = graphics::GameObject::createGameObject();
    building.modelBuffer = model;
    building.transform.translation = pos;
    building.transform.scale = {1.f, 2.f, 1.f};
    gameObjects.emplace(
        building.getId(),
        std::move(building));
}