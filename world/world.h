#pragma once

#include "cubes.h"
#include "core/application.hpp"
#include "buildings.h"

void world(graphics::Application &app)
{
    ground(app);
    road(app);
    sidewalk(app);

    addbuilding(app.getEngineDevice(), {20, 0, 20}, app.getGameObjects());
    addbuilding(app.getEngineDevice(), {20, 0, -20}, app.getGameObjects());
}