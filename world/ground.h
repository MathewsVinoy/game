
#pragma once

#include "graphics/render/model_buffers.hpp"
#include "core/application.hpp"

#include <memory>
#include <vector>

inline void ground(graphics::Application &app)
{
    std::vector<graphics::ModelBuffer::Vertex> gv(4);

    const float S = 500.0f;

    glm::vec3 gp[4] = {
        {-S, 0.0f, -S},
        {-S, 0.0f, S},
        {S, 0.0f, S},
        {S, 0.0f, -S}};

    for (int i = 0; i < 4; ++i)
    {
        gv[i].position = gp[i];
        gv[i].normal = glm::vec3(0.0f, 1.0f, 0.0f);
        gv[i].color = glm::vec3(0.30f, 0.32f, 0.34f);
    }

    std::shared_ptr<graphics::ModelBuffer> model = graphics::ModelBuffer::setbulder(
        app.getEngineDevice(),
        gv,
        {0, 1, 2, 0, 2, 3});

    auto groundObject = graphics::GameObject::createGameObject();

    groundObject.modelBuffer = model;
    groundObject.color = glm::vec3(0.30f, 0.32f, 0.34f);

    app.getGameObjects().emplace(
        groundObject.getId(),
        std::move(groundObject));
}
