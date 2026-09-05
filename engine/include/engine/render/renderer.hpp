#pragma once

#include "engine/core/window.hpp"
#include "engine/render/devices.hpp"
#include "engine/render/swap_chain.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace engine
{
    class Renderer
    {
    public:
        Renderer(Window &window, EngineDevice &device);
        ~Renderer();

        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;

        VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
        float getAspectRatio() const { return swapChain->extentAspectRatio(); }
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const
        {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            assert(currentFrameIndex >= 0 &&
                   static_cast<size_t>(currentFrameIndex) < commandBuffers.size() &&
                   "Current frame index is out of bounds for command buffers");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        Window &window;
        EngineDevice &engineDevice;
        std::unique_ptr<SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex = 0;
        int currentFrameIndex = 0;
        bool isFrameStarted = false;
    };
}