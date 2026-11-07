#pragma once

#include "cubes.h"
#include "core/application.hpp"
#include "player/npc.h"
#include "buildings.h"

void world(graphics::Application &app)
{
    ground(app);
    road(app);
    sidewalk(app);
    npc_init(app);
    npc_init(app, -0.5f
    );

    addbuilding(app, {20, 0, 20});
    addbuilding(app, {20, 0, -20});
}