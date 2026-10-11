#pragma once

#include "graphics/render/model_buffers.hpp"
#include "core/application.hpp"

void ground(graphics::Application &app)
{
    std::vector<graphics::ModelBuffer::Vertex> gv(4);

    const float S = 100000.f;
    glm::vec3 gp[4] = {
        {-S, 0.f, -S},
        {-S, 0.f, S},
        {S, 0.f, S},
        {S, 0.f, -S}};
    for (int i = 0; i < 4; i++)
    {
        gv[i].position = gp[i];
    }
    std::shared_ptr<graphics::ModelBuffer> model =
        graphics::ModelBuffer::setbulder(
            app.getEngineDevice(),
            gv,
            {0, 1, 2, 0, 2, 3});
    auto ground = graphics::GameObject::createGameObject();
    ground.modelBuffer = model;
    ground.color = glm::vec3(0.3f);
    app.getGameObjects().emplace(
        ground.getId(),
        std::move(ground));
}