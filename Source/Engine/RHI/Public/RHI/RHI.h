#pragma once

#include "Core/Logging.h"

// #include <vk_mem_alloc.h>
#include "RHI/VulkanLoader.h"

#define vk_check(expr)                        \
    do {                                      \
        VkResult res = (expr);                \
        if(res != VK_SUCCESS) {               \
            RE_LOGE("Vulkan error: {}", res); \
            __debugbreak();                   \
        }                                     \
    } while(false)

constexpr static const int RE_VK_REQUIRED_VERSION_MAJOR = 1;
constexpr static const int RE_VK_REQUIRED_VERSION_MINOR = 1;

extern PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;

namespace RE {
class RE_RHI_EXPORT FRHI {
    friend class FRenderer;

public:
    explicit FRHI(void *nativeWindow);

    ~FRHI();

    VkPhysicalDevice GetPhysicalDevice() const { return DeviceInfo.PhysicalDevice; }
    VkDevice GetDevice() const { return DeviceInfo.Device; }
    VkSurfaceKHR GetSurface() const { return DeviceInfo.Surface; }
    uint32_t GetGraphicsQueueIndex() const { return DeviceInfo.GraphicsQueueIndex; }
    VkInstance GetInstance() const { return DeviceInfo.Instance; }

protected:
    void CreateInstance();

    void CreateDevice();

    void CreateSurface(void *nativeWindow);

    struct FDeviceInfo {
        VkInstance Instance{VK_NULL_HANDLE};
        VkPhysicalDevice PhysicalDevice{VK_NULL_HANDLE};
        VkDevice Device{VK_NULL_HANDLE};
        uint32_t GraphicsQueueIndex{0xFFFFFFFF};

        VkSurfaceKHR Surface{VK_NULL_HANDLE};
        VkQueue GraphicsQueue{VK_NULL_HANDLE};

        VkPhysicalDeviceMemoryProperties MemoryProperties = {};
        VkPhysicalDeviceProperties PhysicalDeviceProperties = {};
        VkPhysicalDeviceFeatures PhysicalDeviceFeatures = {};

    } DeviceInfo{};

    struct FDebugReportInfo {
        VkDebugUtilsMessengerEXT DebugMessenger{VK_NULL_HANDLE};
        VkDebugReportCallbackEXT DebugReportCallback{VK_NULL_HANDLE};
    } DebugReportInfo{};
};
}