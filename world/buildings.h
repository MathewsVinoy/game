#pragma once

#include "core/application.hpp"

inline void addbuilding(
    graphics::Application &app,
    glm::vec3 pos)
{
    graphics::TransformComponent transform{};
    transform.name = "Building";
    transform.translation = pos;
    transform.scale = {0.01f, 0.01f, 0.01f};

    app.loadStaticModel(
        "assets/models/buildings/uploads_files_2720101_BusGameMap.obj",
        transform);
}