#pragma once

#include "graphics/render/model_buffers.hpp"
#include "core/application.hpp"

float width = 8.0f;
float length = 100.0f;
float thinkness = 0.01f;

inline void road(graphics::Application &app)
{
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