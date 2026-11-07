#pragma once

#include "cubes.h"
#include "core/application.hpp"
#include "buildings.h"

void world(graphics::Application &app)
{
    ground(app);
    road(app);
    sidewalk(app);

    addbuilding(app, {20, 0, 20});
    addbuilding(app, {20, 0, -20});
}