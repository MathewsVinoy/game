# Open Game Project Status Report

## Overview

This project is a custom Vulkan-based 3D engine prototype with a separate game layer. The codebase is already past the basic setup stage and now contains a working rendering stack, model loading, camera control, lighting, descriptor handling, and frame management.

The project is organized into these major areas:

- `engine/` contains the reusable engine runtime and rendering systems.
- `game/` contains the game-facing wrapper and entry point.
- `assets/` stores models and textures used by the engine.
- `shaders/` and `engine/shaders/` store shader sources and compiled SPIR-V output.
- `docs/` stores project notes and documentation.
- `build/` stores generated CMake build output.

The current engine build is in a working state and successfully links the executable.

## Recent changes (scanned 2026-09-09)

- Several new game subsystems were added under `game/`: `World`, `SceneManager`, `ObjectManager`, `InputManager`, and `Character` (see `game/include/game/**`).
- The engine application `loadGameObjects()` now loads a character model (`Spiderman_Amazing_Rigged.obj`) and places a character game object in the scene (`engine/src/core/application.cpp`).
- Engine device setup now enables validation layers and installs a `VkDebugUtilsMessengerEXT` callback (see `engine/src/render/devices.cpp`).
- Keyboard controller API was extended with `getKeyState()` and additional character-specific key mappings (see `engine/include/engine/core/keyboard_controller.hpp`).
- Some files were removed/renamed (e.g., `engine/include/engine/render/terrain.hpp` and the top-level `engine/src/main.cpp` were deleted in the current tree).
- The build helper `tests/compile.sh` was modified (check it for CI/build checks).

If you want a precise git diff for these items I can include it here or commit the changes to a branch.

## High-Level Status

What is already built:

- Vulkan instance, device, queue, and surface setup.
- Window creation and resize handling through GLFW.
- Swap chain creation, framebuffers, render pass, depth resources, and synchronization.
- Command buffer management and frame begin/end control.
- Graphics pipeline creation for mesh rendering and point lights.
- Uniform buffer and descriptor set management.
- OBJ model loading and GPU buffer uploads.
- Camera projection and view math.
- Keyboard camera movement.
- Game object transforms and point-light objects.
- Separate render systems for meshes and point lights.

What is still minimal or incomplete:

- The `game/` folder is only a thin wrapper right now.
- Game logic methods are placeholders.
- There is no UI, audio, physics, animation, or mission/gameplay system yet.
- The project is still an engine foundation, not a full game.

## Project Structure

### Top Level Files

- `CMakeLists.txt` controls the overall build and adds the engine subproject.
- `README.md` is currently empty.
- `build/` contains generated build files and should be treated as output, not source.

### `docs/`

- `docs/tree.md` is a project tree snapshot.
- `docs/draft1.md` contains a long-form conceptual note about building a custom engine for a GTA-style game.
- `docs/draft2.md` contains guidance on what to do next in Vulkan development.
- `docs/project_status_report.md` is this status and architecture report.

### `assets/`

- `assets/models/` stores imported 3D models and material files.
- `assets/textures/` stores texture assets.

These files are consumed by the model loader and rendering systems.

### `shaders/`

- This folder holds shader sources and compiled shader binaries used by the engine.
- The engine CMake file compiles `.vert` and `.frag` files into `.spv` files automatically.

### `engine/`

This is the main engine implementation. It is split into headers under `engine/include/engine/` and source files under `engine/src/`.

### `game/`

This is the game-facing layer. Recent additions make this folder a lightweight game framework that will host gameplay code and scene management. Key submodules:

- `game/include/game/world/world.hpp` and `game/src/world/` — `World` aggregates `SceneManager` and `ObjectManager`, and exposes `initialize()`, `update(float)`, and `shutdown()`.
- `game/include/game/world/scene_manager.hpp` — `SceneManager` loads and holds the current `Scene` object and provides `loadScene(const std::string&)` and `getCurrentScene()`.
- `game/include/game/objects/object_manager.hpp` — `ObjectManager` creates and tracks in-game `Object` instances, provides `createObject()`, `getObject()`, `removeObject()`, and `hasObject()`; it uses a simple `ObjectID` and vector storage.
- `game/include/game/input/input_manager.hpp` — `InputManager` wraps the engine keyboard controller and provides `isKeyDown()` and `isKeyPressed()` convenience methods.
- `game/include/game/character/character.hpp` — `Character` is a simple gameplay entity wrapper over `Object` with a `move()` helper and `moveSpeed` parameter.

These modules are designed to sit above the engine and use engine primitives (game objects, transforms, model buffers, and input) rather than duplicating engine behavior.

## Engine Core Files

### `engine/include/engine/core/engine.hpp`

Defines the top-level `Engine` wrapper. It owns the application object and provides the public lifecycle methods `initialize()`, `run()`, and `shutdown()`.

### `engine/src/core/engine.cpp`

Implements the top-level engine wrapper. It creates the application, starts the run loop, and releases the application on shutdown.

### `engine/include/engine/core/application.hpp`

Defines the main application class. This is where the window, device, renderer, descriptor pool, and scene objects are owned.

### `engine/src/core/application.cpp`

Implements the actual engine runtime loop. In the latest code it:

- Allocates per-frame uniform buffers and maps them for CPU writes.
- Builds a global descriptor set layout and per-frame descriptor sets for the global UBO.
- Creates `RenderSystem` and `PointLightSystem` instances and a `Camera`.
- Loads game objects (now includes a character model and a point light).
- Handles input polling and moves a `viewerObject` via `KeyboardMovementController`.
- Prepares `FrameInfo`, updates `GlobalUbo` with projection/view/inverse view and point-light data, writes/flushes the UBO, and issues rendering via `RenderSystem` and `PointLightSystem`.

Note: `loadGameObjects()` now instantiates a character from `assets/models/Spiderman_Amazing_Rigged.obj` and places it at the world origin. This demonstrates how the `game/` layer and `engine/` layer interact.

### `engine/include/engine/core/window.hpp`

Declares the GLFW window wrapper. It manages window size, resize state, surface creation, and the native GLFW window handle.

### `engine/src/core/window.cpp`

Implements window creation, resize callbacks, and Vulkan surface creation.

### `engine/include/engine/core/descriptors.hpp`

Declares descriptor set layout, descriptor pool, and descriptor writer helpers.

### `engine/src/core/descriptors.cpp`

Implements descriptor allocation and writing for uniform buffers and textures.

### `engine/include/engine/core/keyboard_controller.hpp`

Defines keyboard input mappings and the camera movement controller.

### `engine/src/core/keyboard_controller.cpp`

Implements keyboard-driven camera rotation and movement in 3D space.

### `engine/include/engine/core/frame_info.hpp`

Defines the per-frame data package used by render systems. It contains command buffer, camera, descriptor set, frame index, frame time, and game objects.

## Rendering Files

### `engine/include/engine/render/devices.hpp`

Declares the Vulkan device wrapper. It handles instance creation, device selection, queues, command pools, buffer creation, image creation, and swap chain queries.

### `engine/src/render/devices.cpp`

Implements Vulkan initialization and low-level helper functions for memory, buffers, images, and queue management. Recent changes:

- A `VkDebugUtilsMessengerEXT` helper and `debugCallback` were added to forward validation layer messages to stderr.
- Instance creation now populates and enables validation layers when `enableValidationLayers` is true and wires the debug messenger into instance creation (`populateDebugMessengerCreateInfo`).

This is useful for development: when you build/run with validation enabled you will receive runtime diagnostics about incorrect Vulkan usage.

### `engine/include/engine/render/buffer.hpp`

Declares the GPU buffer abstraction used for vertex buffers, index buffers, and uniform buffers.

### `engine/src/render/buffer.cpp`

Implements buffer creation, mapping, flushing, invalidation, and descriptor info access.

### `engine/include/engine/render/camera.hpp`

Declares the camera class and its projection and view controls.

### `engine/src/render/camera.cpp`

Implements camera math for orthographic and perspective projection, target-based view, direction-based view, and YXZ view rotation.

### `engine/include/engine/render/object.hpp`

Declares `GameObject`, `TransformComponent`, and `PointLightComponent`.

### `engine/src/render/object.cpp`

Implements transform matrix and normal matrix generation, plus construction of point-light objects.

### `engine/include/engine/render/model_buffers.hpp`

Declares the model buffer and vertex structures used to store mesh data on the GPU.

### `engine/src/render/model_buffer.cpp`

Implements OBJ loading, vertex deduplication, vertex buffer creation, index buffer creation, and draw/bind logic.

### `engine/include/engine/render/pipeline.hpp`

Declares the Vulkan graphics pipeline wrapper and the pipeline configuration structure.

### `engine/src/render/pipeline.cpp`

Implements shader loading, shader module creation, pipeline creation, pipeline binding, and default pipeline state setup.

### `engine/include/engine/render/renderer.hpp`

Declares the renderer, which owns the swap chain and command buffers and controls frame flow.

### `engine/src/render/renderer.cpp`

Implements frame acquisition, command buffer recording, render pass begin and end, swap chain recreation, and frame submission.

### `engine/include/engine/render/swap_chain.hpp`

Declares the swap chain wrapper and all swap chain images, image views, framebuffers, synchronization, and render pass data.

### `engine/src/render/swap_chain.cpp`

Implements swap chain creation, image view creation, depth resource creation, render pass setup, framebuffer creation, semaphores, fences, image acquisition, and presentation.

### `engine/include/engine/render/utils.hpp`

Provides helper utilities used by the renderer, such as hashing support for model data.

## Rendering Systems

### `engine/include/engine/system/render_system.hpp`

Declares the main mesh rendering system.

### `engine/src/system/render_system.cpp`

Implements the object rendering pass. It binds the mesh pipeline, binds descriptor sets, pushes transform data, and draws model buffers for each renderable game object.

### `engine/include/engine/system/point_light.hpp`

Declares the point-light rendering system and the global lighting buffer structure.

### `engine/src/system/point_light.cpp`

Implements point-light animation, lighting buffer updates, and visible point-light drawing. The system:

- Iterates scene `GameObject`s that contain point-light components.
- Rotates or updates their positions per-frame and copies position/color/intensity into the `GlobalUbo`.
- Renders visible light primitives via push constants and a separate pipeline.

## Game Layer Files

### `game/include/game/game.hpp`

Declares the game-facing `Game` class. Recent work added concrete subsystems under `game/` that will be used to implement game flow. The `Game` class is intended to orchestrate `World`, `InputManager`, and high-level systems like UI and scripting as they are implemented.

### `game/src/game.cpp`

Implements the `Game` lifecycle methods. At the moment the methods are empty stubs, which means the game layer is ready for future gameplay code but not yet populated.

### `game/src/main.cpp`

Defines the game executable entry point. It constructs the engine, initializes it, runs it, and then shuts it down.

## How The Engine Works

The runtime flow is roughly:

1. The application creates the window and Vulkan device.
2. The swap chain and renderer are initialized.
3. Models are loaded from assets and stored as game objects.
4. Each frame, input updates the camera controller.
5. Camera matrices are written into the uniform buffer.
6. The render system draws all mesh objects.
7. The point-light system updates and draws lights.
8. The renderer ends the frame and presents the image.

This creates a classic real-time graphics loop with a clear separation between engine, rendering systems, and scene data.

## Build & Run (quick)

From the project root run:

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
./build/engine/engine   # or the game binary if created under build/game/
```

If validation layers are enabled and the Vulkan SDK is installed, you will see validation messages on stderr (helpful during development).

## Recommended Next Documentation Additions

- Add a `docs/README.md` with step-by-step build instructions and dependencies (Vulkan SDK, GLFW, GLM).
- Add a short API reference for `game/` classes that includes examples for creating objects, moving characters, and switching scenes.
- Add a CONTRIBUTING.md with the project's expected workflow and how to run the CI/test helper `tests/compile.sh`.

## Current Strengths

- The project already has a strong modular layout.
- Rendering is split into reusable systems instead of one large monolithic file.
- The code supports both meshes and point lights.
- Asset loading is real, not mocked.
- The build is configured to compile shaders automatically.
- The engine is organized so the game layer can grow on top of it.

## Current Gaps

- The game layer is mostly empty.
- There is no gameplay loop beyond scene rendering.
- There is no UI, physics, animation, or audio system yet.
- The docs folder contains notes, but the main README is still empty.

## Suggested Next Steps

- Add a real game loop in `game/` that uses the engine as a reusable library.
- Expand the docs with setup instructions and a file-by-file architecture map.
- Add gameplay systems such as input-driven player state, UI, and scene management.
- Fill the README with a short project summary and build instructions.
