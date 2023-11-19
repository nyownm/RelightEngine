#pragma once
#include "re-render_export.h"
#include "RHI/RHI.h"

namespace RE {
class RE_RENDER_EXPORT FRenderer {
public:
    FRenderer(void *nativeWindow);
    ~FRenderer();
    void Tick();

    FRHI RHI;

private:
    void CreateSwapchain();
    void createRenderPass();
    void createFramebuffers();
    void createCommandBuffer();
    void createSyncObjects();

    struct FSwapchainInfo {
        VkSwapchainKHR Swapchain{VK_NULL_HANDLE};
        VkFormat SurfaceFormat;
        std::vector<VkImage> SwapchainImages;
        std::vector<VkImageView> SwapchainImageViews;
        VkExtent2D SwapchainExtent;
        std::vector<VkFramebuffer> Framebuffers;
    } SwapchainInfo{};

    struct RenderPassInfo {
        VkRenderPass vkRenderPass{VK_NULL_HANDLE};
        VkCommandPool vkCommandPool{VK_NULL_HANDLE};
        std::vector<VkCommandBuffer> vkCommandBuffers;
        std::vector<VkSemaphore> vkImageAvailableSemaphores;
        std::vector<VkSemaphore> vkRenderFinishedSemaphores;
        std::vector<VkFence> vkInFlightFences;
        size_t currentFrame{0};
        const size_t MAX_FRAMES_IN_FLIGHT{2};
    } renderPassInfo;
};

}