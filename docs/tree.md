mygame-project/
├── CMakeLists.txt                 # top-level build config
├── .gitignore
├── README.md
│
├── engine/                        # the reusable engine — no game-specific code here
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── engine/
│   │       ├── core/
│   │       │   ├── application.h
│   │       │   ├── window.h
│   │       │   ├── logger.h
│   │       │   ├── input.h
│   │       │   ├── timer.h
│   │       │   └── allocator.h
│   │       ├── renderer/
│   │       │   ├── vk_context.h        # instance, device, queues
│   │       │   ├── vk_swapchain.h
│   │       │   ├── vk_pipeline.h
│   │       │   ├── vk_buffer.h
│   │       │   ├── vk_texture.h
│   │       │   ├── mesh.h
│   │       │   ├── material.h
│   │       │   ├── camera.h
│   │       │   └── renderer.h
│   │       ├── scene/
│   │       │   ├── ecs.h               # entity component system
│   │       │   ├── components.h
│   │       │   ├── transform.h
│   │       │   └── scene_graph.h
│   │       ├── physics/
│   │       │   ├── collision.h
│   │       │   └── rigidbody.h
│   │       ├── world/
│   │       │   ├── streaming.h         # chunk load/unload
│   │       │   ├── terrain.h
│   │       │   └── spatial_grid.h      # or octree.h
│   │       ├── audio/
│   │       │   └── audio_engine.h
│   │       ├── resources/
│   │       │   ├── asset_manager.h
│   │       │   ├── model_loader.h
│   │       │   └── shader_compiler.h
│   │       └── platform/
│   │           └── file_system.h
│   │
│   ├── src/                       # mirrors include/ structure, .cpp files here
│   │   ├── core/
│   │   ├── renderer/
│   │   ├── scene/
│   │   ├── physics/
│   │   ├── world/
│   │   ├── audio/
│   │   ├── resources/
│   │   └── platform/
│   │
│   └── third_party/                # vendored/fetched libraries
│       ├── glfw/
│       ├── glm/
│       ├── stb/                    # image loading
│       └── (later: assimp, jolt, imgui, etc.)
│
├── game/                          # your actual game, uses the engine as a library
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── main.cpp
│   │   ├── player_controller.cpp
│   │   ├── vehicle.cpp
│   │   └── npc_ai.cpp
│   └── include/
│       └── game/
│
├── shaders/
│   ├── src/                       # .vert/.frag/.comp source
│   │   ├── basic.vert
│   │   └── basic.frag
│   └── compiled/                  # .spv output (gitignored, built by CMake)
│
├── assets/
│   ├── models/
│   ├── textures/
│   ├── audio/
│   └── levels/
│
├── tools/                         # your future level editor lives here eventually
│   └── editor/
│
├── tests/
│   └── (unit tests per module, e.g. using Catch2 or doctest)
│
└── build/                          # gitignored — CMake output