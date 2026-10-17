#pragma once

#include "object/object.hpp"

void addbuilding(graphics::EngineDevice &device, glm::vec3 pos, graphics::GameObject::Map &gameObjects)
{

    std::shared_ptr<graphics::ModelBuffer> model = graphics::ModelBuffer::createModelFromFile(device,
                                                                                              "assets/models/uploads_files_2720101_BusGameMap.obj");
    auto building = graphics::GameObject::createGameObject();
    building.modelBuffer = model;
    building.transform.name = "Cube";
    building.transform.translation = pos;
    building.transform.scale = {0.01f, 0.01f, 0.01f};
    gameObjects.emplace(
        building.getId(),
        std::move(building));
}