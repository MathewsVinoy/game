#include "npc_manager.hpp"

namespace opengame
{

    void NPC_Manager::init(graphics::Application &app)
    {
        NPC_Objectc npc1;
        npc1.type = "Walk";
        npc1.start_point = -50.f;
        npc1.end_point = 50.f;
        npc1.direction = 1.0f;
        NPC npc1_instance(app, 5.f, npc1);
        npcs.push_back(std::move(npc1_instance));
        NPC npc2_instance(app, -5.f, npc1);
        npcs.push_back(std::move(npc2_instance));
        npc1.type = " ";
        NPC npc3_instance(app, 0.f, npc1);
        npcs.push_back(std::move(npc3_instance));
    }

    void NPC_Manager::update(float deltaTime)
    {
        for (auto &npc : npcs)
        {
            if (npc.getType() == "Walk")
            {
                npc.simple_walk(deltaTime);
            }
            else
            {
                npc.normal(deltaTime);
            }
        }
    }
}