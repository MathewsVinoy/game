#pragma once

#include "cubes.h"
#include "core/application.hpp"

void world(graphics::Application &app)
{
    addCube(app.getEngineDevice(), {10.f, 0.f, 10.f}, app.getGameObjects());
    addCube(app.getEngineDevice(), {15.f, 0.f, 10.f}, app.getGameObjects());
    addCube(app.getEngineDevice(), {-15.f, 0.f, 10.f}, app.getGameObjects());
    addCube(app.getEngineDevice(), {10.f, 0.f, 15.f}, app.getGameObjects());
    addCube(app.getEngineDevice(), {10.f, 0.f, -15.f}, app.getGameObjects());
}