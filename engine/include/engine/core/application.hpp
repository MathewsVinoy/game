#pragma once

#include "engine/core/window.hpp"
#include "engine/render/pipeline.hpp"
#include "engine/render/devices.hpp"
#include "engine/render/swap_chain.hpp"

#include <memory>
#include <vector>

namespace engine
{
    class Application
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        Application();
        ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        void run();

    private:
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        Window window{WIDTH, HEIGHT, "Engine"};
        EngineDevice engineDevice{window};
        SwapChain swapChain{engineDevice, window.getExtent()};
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
    };
}