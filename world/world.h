#pragma once

#include "cubes.h"
#include "core/application.hpp"
#include "player/npc/npc.hpp"
#include "buildings.h"

void world(graphics::Application &app)
{
    ground(app);
    road(app);
    sidewalk(app);
    opengame::NPC npc;
    npc.addNPC(1, "Alice", graphics::TransformComponent{}, app, 4.f);
    npc.addNPC(2, "Bob", graphics::TransformComponent{}, app, -4.f);

    addbuilding(app, {20, 0, 20});
    addbuilding(app, {20, 0, -20});
}