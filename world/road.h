#pragma once

#include "graphics/render/model_buffers.hpp"
#include "core/application.hpp"
#include "cubes.h"

float length = 100.0f;

inline void road(graphics::Application &app)
{
    float width = 8.0f;
    float thinkness = 0.01f;
    std::vector<graphics::ModelBuffer::Vertex> vertices(4);
    const float halfWidth = width * 0.5f;
    const float halfLength = length * 0.5f;
    const glm::vec3 positions[4] = {
        {-halfWidth, thinkness, -halfLength},
        {halfWidth, thinkness, -halfLength},
        {halfWidth, thinkness, halfLength},
        {-halfWidth, thinkness, halfLength}};
    const glm::vec2 uvs[4] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}};
    for (int i = 0; i < 4; ++i)
    {
        vertices[i].position = positions[i];
        vertices[i].color = glm::vec3(0.18f, 0.18f, 0.18f);
        vertices[i].normal = glm::vec3(0.0f, 1.0f, 0.0f);
        vertices[i].uv = uvs[i];
    }

    std::vector<uint32_t> indices = {
        0, 2, 1,
        0, 3, 2};
    std::shared_ptr<graphics::ModelBuffer> model = graphics::ModelBuffer::setbulder(
        app.getEngineDevice(),
        vertices,
        indices);

    auto roadObject = graphics::GameObject::createGameObject();
    roadObject.modelBuffer = model;
    roadObject.color = glm::vec3(0.18f, 0.18f, 0.18f);

    app.getGameObjects().emplace(
        roadObject.getId(),
        std::move(roadObject));
}

void sidewalk(graphics::Application &app)
{
    std::shared_ptr<graphics::ModelBuffer> model = createBoxSurface(
        app.getEngineDevice(),
        2.0f,
        100.0f,
        glm::vec3(0.65f, 0.65f, 0.62f));
    auto leftSidewalk = graphics::GameObject::createGameObject();
    leftSidewalk.transform.name = "LeftSidewalk";
    leftSidewalk.transform.translation = {-5.15f, 0.03f, 0.0f};
    leftSidewalk.modelBuffer = model;
    app.getGameObjects().emplace(leftSidewalk.getId(), std::move(leftSidewalk));

    auto rightSidewalk = graphics::GameObject::createGameObject();
    rightSidewalk.transform.name = "RightSidewalk";
    rightSidewalk.transform.translation = {5.15f, 0.03f, 0.0f};
    rightSidewalk.modelBuffer = model;
    app.getGameObjects().emplace(rightSidewalk.getId(), std::move(rightSidewalk));
}