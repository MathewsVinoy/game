#pragma once

#include "object/object.hpp"

void addCube(graphics::EngineDevice &device, glm::vec3 pos, graphics::GameObject::Map &gameObjects)
{

    std::shared_ptr<graphics::ModelBuffer> model = graphics::ModelBuffer::createModelFromFile(device, "assets/models/cube.obj");
    auto building = graphics::GameObject::createGameObject();
    building.modelBuffer = model;
    building.transform.name = "Cube";
    building.transform.translation = pos;
    building.transform.scale = {1.f, 1.f, 1.f};
    building.transform.aabb = physics::Collision().createAABB(pos, {1.f, 1.f, 1.f});
    gameObjects.emplace(
        building.getId(),
        std::move(building));
}

std::shared_ptr<graphics::ModelBuffer> createBoxSurface(graphics::EngineDevice &device, float width, float length, glm::vec3 color)
{
    std::vector<graphics::ModelBuffer::Vertex> vertices(4);

    const float x = width * 0.5f;
    const float z = length * 0.5f;

    const glm::vec3 positions[4] = {
        {-x, 0.0f, -z},
        {x, 0.0f, -z},
        {x, 0.0f, z},
        {-x, 0.0f, z}};

    const glm::vec2 uvs[4] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}};

    for (int i = 0; i < 4; ++i)
    {
        vertices[i].position = positions[i];
        vertices[i].color = color;
        vertices[i].normal = {0.0f, 1.0f, 0.0f};
        vertices[i].uv = uvs[i];
    }

    std::vector<uint32_t> indices = {
        0, 2, 1,
        0, 3, 2};

    return graphics::ModelBuffer::setbulder(
        device,
        std::move(vertices),
        std::move(indices));
}

// std::vector<glm::vec3> position_buildings{
// {-15, 0, 10},
// {-5, 0, 10},
// {5, 0, 10},
// {15, 0, 10},
//    //South side
//
// {-15, 0, -10},
// {-5, 0, -10},
// {5, 0, -10},
// {15, 0, -10},
// };