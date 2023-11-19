#include "Render/Renderer.h"

namespace RE {
FRenderer::FRenderer(void *nativeWindow): RHI(nativeWindow) {
    CreateSwapchain();
    createRenderPass();
    createFramebuffers();
    createCommandBuffer();
    createSyncObjects();
}
FRenderer::~FRenderer() {
    if(RHI.GetDevice() != VK_NULL_HANDLE) { vkDeviceWaitIdle(RHI.GetDevice()); }
    for(const auto &fence: renderPassInfo.vkInFlightFences) {
        vkDestroyFence(RHI.GetDevice(), fence, nullptr);
    }
    renderPassInfo.vkInFlightFences.clear();
    for(auto &semaphore: renderPassInfo.vkImageAvailableSemaphores) {
        vkDestroySemaphore(RHI.GetDevice(), semaphore, nullptr);
    }
    renderPassInfo.vkImageAvailableSemaphores.clear();
    for(auto &semaphore: renderPassInfo.vkRenderFinishedSemaphores) {
        vkDestroySemaphore(RHI.GetDevice(), semaphore, nullptr);
    }
    renderPassInfo.vkRenderFinishedSemaphores.clear();

    for(auto &commandBuffer: renderPassInfo.vkCommandBuffers) {
        vkFreeCommandBuffers(
            RHI.GetDevice(), renderPassInfo.vkCommandPool, 1, &commandBuffer);
    }
    renderPassInfo.vkCommandBuffers.clear();
    if(renderPassInfo.vkCommandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(RHI.GetDevice(), renderPassInfo.vkCommandPool, nullptr);
        renderPassInfo.vkCommandPool = VK_NULL_HANDLE;
    }
    for(auto &framebuffer: SwapchainInfo.Framebuffers) {
        vkDestroyFramebuffer(RHI.GetDevice(), framebuffer, nullptr);
    }
    SwapchainInfo.Framebuffers.clear();
    for(auto &imageView: SwapchainInfo.SwapchainImageViews) {
        vkDestroyImageView(RHI.GetDevice(), imageView, nullptr);
    }
    SwapchainInfo.SwapchainImageViews.clear();
    if(renderPassInfo.vkRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(RHI.GetDevice(), renderPassInfo.vkRenderPass, nullptr);
        renderPassInfo.vkRenderPass = VK_NULL_HANDLE;
    }
    if(SwapchainInfo.Swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(RHI.GetDevice(), SwapchainInfo.Swapchain, nullptr);
        SwapchainInfo.Swapchain = VK_NULL_HANDLE;
    }
}

void FRenderer::CreateSwapchain() {
    // RHI.createSwapchain();
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR func =
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    vk_check(func(
        RHI.DeviceInfo.PhysicalDevice, RHI.DeviceInfo.Surface, &surfaceCapabilities));

    uint32_t formatCount;
    vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
        RHI.GetPhysicalDevice(), RHI.GetSurface(), &formatCount, nullptr));
    checkf(formatCount > 0, "No surface formats found.");

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
        RHI.GetPhysicalDevice(), RHI.GetSurface(), &formatCount, surfaceFormats.data()));

    uint32_t presentModeCount;
    vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
        RHI.GetPhysicalDevice(), RHI.GetSurface(), &presentModeCount, nullptr));
    checkf(presentModeCount > 0, "No present modes found.");

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
        RHI.GetPhysicalDevice(), RHI.GetSurface(), &presentModeCount,
        presentModes.data()));

    VkSurfaceFormatKHR surfaceFormat = surfaceFormats[0];
    auto preferred_format_list = std::vector<VkFormat>{
        VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_A8B8G8R8_SRGB_PACK32};
    for(const auto &format: preferred_format_list) {
        for(const auto &surface_format: surfaceFormats) {
            if(surface_format.format == format) {
                surfaceFormat = surface_format;
                break;
            }
        }
    }

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for(const auto &mode: presentModes) {
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = mode;
            break;
        }
    }

    VkExtent2D swapchainExtent = surfaceCapabilities.currentExtent;
    if(surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
        swapchainExtent.width = std::max(
            surfaceCapabilities.minImageExtent.width,
            std::min(surfaceCapabilities.maxImageExtent.width, swapchainExtent.width));
        swapchainExtent.height = std::max(
            surfaceCapabilities.minImageExtent.height,
            std::min(surfaceCapabilities.maxImageExtent.height, swapchainExtent.height));
    }

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if(surfaceCapabilities.maxImageCount > 0 &&
       imageCount > surfaceCapabilities.maxImageCount) {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo{
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchainCreateInfo.surface = RHI.GetSurface();
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    vk_check(vkCreateSwapchainKHR(
        RHI.GetDevice(), &swapchainCreateInfo, nullptr, &SwapchainInfo.Swapchain));

    SwapchainInfo.SwapchainExtent = swapchainExtent;
    SwapchainInfo.SurfaceFormat = surfaceFormat.format;

    uint32_t imagesCount;
    vk_check(vkGetSwapchainImagesKHR(
        RHI.GetDevice(), SwapchainInfo.Swapchain, &imageCount, nullptr));
}

void FRenderer::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = SwapchainInfo.SurfaceFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    vk_check(vkCreateRenderPass(
        RHI.GetDevice(), &renderPassCreateInfo, nullptr, &renderPassInfo.vkRenderPass));
}

void FRenderer::createFramebuffers() {

    uint32_t imagesCount;
    vk_check(vkGetSwapchainImagesKHR(
        RHI.GetDevice(), SwapchainInfo.Swapchain, &imagesCount, nullptr));
    SwapchainInfo.SwapchainImages.resize(imagesCount);
    vk_check(vkGetSwapchainImagesKHR(
        RHI.GetDevice(), SwapchainInfo.Swapchain, &imagesCount,
        SwapchainInfo.SwapchainImages.data()));

    SwapchainInfo.SwapchainImageViews.resize(imagesCount);
    for(uint32_t i = 0; i < imagesCount; i++) {
        VkImageViewCreateInfo imageViewCreateInfo{
            VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        imageViewCreateInfo.image = SwapchainInfo.SwapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = SwapchainInfo.SurfaceFormat;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        vk_check(vkCreateImageView(
            RHI.GetDevice(), &imageViewCreateInfo, nullptr,
            &SwapchainInfo.SwapchainImageViews[i]));
    }

    SwapchainInfo.Framebuffers.resize(imagesCount);
    for(uint32_t i = 0; i < imagesCount; i++) {
        VkImageView attachments[] = {SwapchainInfo.SwapchainImageViews[i]};

        VkFramebufferCreateInfo framebufferCreateInfo{
            VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        framebufferCreateInfo.renderPass = renderPassInfo.vkRenderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = attachments;
        framebufferCreateInfo.width = SwapchainInfo.SwapchainExtent.width;
        framebufferCreateInfo.height = SwapchainInfo.SwapchainExtent.height;
        framebufferCreateInfo.layers = 1;

        vk_check(vkCreateFramebuffer(
            RHI.GetDevice(), &framebufferCreateInfo, nullptr,
            &SwapchainInfo.Framebuffers[i]));
    }
}

void FRenderer::createCommandBuffer() {
    VkCommandPoolCreateInfo commandPoolCreateInfo{
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    commandPoolCreateInfo.queueFamilyIndex = RHI.GetGraphicsQueueIndex();
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vk_check(vkCreateCommandPool(
        RHI.GetDevice(), &commandPoolCreateInfo, nullptr, &renderPassInfo.vkCommandPool));

    renderPassInfo.vkCommandBuffers.resize(SwapchainInfo.Framebuffers.size());

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    commandBufferAllocateInfo.commandPool = renderPassInfo.vkCommandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount =
        static_cast<uint32_t>(renderPassInfo.vkCommandBuffers.size());

    vk_check(vkAllocateCommandBuffers(
        RHI.GetDevice(), &commandBufferAllocateInfo,
        renderPassInfo.vkCommandBuffers.data()));

    for(uint32_t i = 0; i < renderPassInfo.vkCommandBuffers.size(); i++) {
        VkCommandBufferBeginInfo commandBufferBeginInfo{
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        vk_check(vkBeginCommandBuffer(
            renderPassInfo.vkCommandBuffers[i], &commandBufferBeginInfo));

        VkRenderPassBeginInfo renderPassBeginInfo{
            VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        renderPassBeginInfo.renderPass = renderPassInfo.vkRenderPass;
        renderPassBeginInfo.framebuffer = SwapchainInfo.Framebuffers[i];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = SwapchainInfo.SwapchainExtent;

        VkClearValue clearColor = {0.0f, 1.0f, 0.0f, 1.0f};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(
            renderPassInfo.vkCommandBuffers[i], &renderPassBeginInfo,
            VK_SUBPASS_CONTENTS_INLINE);

        vkCmdEndRenderPass(renderPassInfo.vkCommandBuffers[i]);

        vk_check(vkEndCommandBuffer(renderPassInfo.vkCommandBuffers[i]));
    }
}
void FRenderer::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkFenceCreateInfo fenceCreateInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    renderPassInfo.vkImageAvailableSemaphores.resize(renderPassInfo.MAX_FRAMES_IN_FLIGHT);
    renderPassInfo.vkRenderFinishedSemaphores.resize(renderPassInfo.MAX_FRAMES_IN_FLIGHT);
    renderPassInfo.vkInFlightFences.resize(renderPassInfo.MAX_FRAMES_IN_FLIGHT);

    for(uint32_t i = 0; i < renderPassInfo.MAX_FRAMES_IN_FLIGHT; i++) {
        vk_check(vkCreateSemaphore(
            RHI.GetDevice(), &semaphoreCreateInfo, nullptr,
            &renderPassInfo.vkImageAvailableSemaphores[i]));
        vk_check(vkCreateSemaphore(
            RHI.GetDevice(), &semaphoreCreateInfo, nullptr,
            &renderPassInfo.vkRenderFinishedSemaphores[i]));
        vk_check(vkCreateFence(
            RHI.GetDevice(), &fenceCreateInfo, nullptr,
            &renderPassInfo.vkInFlightFences[i]));
    }
}
}
