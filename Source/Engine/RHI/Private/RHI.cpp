#include "RHI/RHI.h"

#include <algorithm>
#include <unordered_set>

namespace RE {
FRHI::FRHI(void *nativeWindow) {
    InitializeVulkanLoader();

    CreateInstance();

    CreateDevice();

    CreateSurface(nativeWindow);
}
FRHI::~FRHI() {
    if(DeviceInfo.Device != VK_NULL_HANDLE) { vkDeviceWaitIdle(DeviceInfo.Device); }

    if(DebugReportInfo.DebugMessenger != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(
            DeviceInfo.Instance, DebugReportInfo.DebugMessenger, nullptr);
    }
    if(DebugReportInfo.DebugReportCallback != VK_NULL_HANDLE) {
        vkDestroyDebugReportCallbackEXT(
            DeviceInfo.Instance, DebugReportInfo.DebugReportCallback, nullptr);
    }
    if(DeviceInfo.Surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(DeviceInfo.Instance, DeviceInfo.Surface, nullptr);
    }

    if(DeviceInfo.Device != VK_NULL_HANDLE) {
        vkDestroyDevice(DeviceInfo.Device, nullptr);
    }

    if(DeviceInfo.Instance != VK_NULL_HANDLE) {
        vkDestroyInstance(DeviceInfo.Instance, nullptr);
    }
}

namespace {
VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    RE_LOGD("validation layer: {}", pCallbackData->pMessage);

    return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallbackFunc(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type, uint64_t object,
    size_t location, int32_t message_code, const char *layer_prefix, const char *message,
    void *user_data) {
    if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        RE_LOGD("Validation Layer: Error: {} {}", layer_prefix, message);
    } else if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        RE_LOGD("Validation Layer: Warning: {} {}", layer_prefix, message);
    } else if(flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        RE_LOGD("Validation Layer: Performance warning: {} {}", layer_prefix, message);
    } else {
        RE_LOGD("Validation Layer: Information: {} {}", layer_prefix, message);
    }
    return VK_FALSE;
}

template<typename T, typename FieldType>
bool validateExtensions(
    const std::vector<const char *> &required, const std::vector<T> &available,
    FieldType T::*Name) {
    for(auto Extension: required) {
        bool Found = false;
        for(auto &AvailableExtension: available) {
            if(strcmp(AvailableExtension.*Name, Extension) == 0) {
                Found = true;
                break;
            }
        }

        if(!Found) { return false; }
    }

    return true;
}
}

using FExtensionSet = std::unordered_set<std::string_view>;

FExtensionSet GetInstanceExtensions() {
    std::vector<std::string_view> RequestedExtensions{
        // Request all cross-platform extensions.
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,

        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME};

#if RE_VALIDATION_LAYERS
    // Request these if available.
    RequestedExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    RequestedExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    uint32_t ExtensionCount;
    vk_check(vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr));
    std::vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
    vk_check(vkEnumerateInstanceExtensionProperties(
        nullptr, &ExtensionCount, AvailableExtensions.data()));

    FExtensionSet Extensions;

    for(auto const &ExtensionProps: AvailableExtensions) {
        for(auto const &TargetExtension: RequestedExtensions) {
            if(TargetExtension == ExtensionProps.extensionName) {
                Extensions.insert(TargetExtension);
            }
        }
    }
#if RE_VALIDATION_LAYERS
    if(Extensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        Extensions.erase(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
#endif

    return Extensions;
}

FExtensionSet GetRequiredInstanceExtensions() {
    FExtensionSet Extensions;
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    Extensions.insert(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    Extensions.insert(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    Extensions.insert(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    Extensions.insert(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    Extensions.insert(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    Extensions.insert(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
    Extensions.insert(VK_KHR_DISPLAY_EXTENSION_NAME);
#else
    #pragma error Platform not supported
#endif
    return Extensions;
}

void FRHI::CreateInstance() {

    FExtensionSet InstanceExtensions = GetInstanceExtensions();
    InstanceExtensions.merge(GetRequiredInstanceExtensions());

    std::vector<const char *> RequestedLayers({
#if RE_VALIDATION_LAYERS
        "VK_LAYER_KHRONOS_validation",
    #if RE_DEBUG_DUMP_API
        "VK_LAYER_LUNARG_api_dump",
    #endif
    #if defined(ENABLE_RENDERDOC)
        "VK_LAYER_RENDERDOC_Capture",
    #endif
#endif
    });

#if RE_VALIDATION_LAYERS
    uint32_t LayerCount;
    vk_check(vkEnumerateInstanceLayerProperties(&LayerCount, nullptr));

    std::vector<VkLayerProperties> SupportedValidationLayers(LayerCount);
    vk_check(vkEnumerateInstanceLayerProperties(
        &LayerCount, SupportedValidationLayers.data()));

    checkf(
        validateExtensions(
            RequestedLayers, SupportedValidationLayers, &VkLayerProperties::layerName),
        "Required validation layers are missing.");
#endif

    std::vector<const char *> RequiredExtensions;
    RequiredExtensions.reserve(InstanceExtensions.size());
    for(auto const &Extension: InstanceExtensions) {
        RequiredExtensions.push_back(Extension.data());
    }

    VkApplicationInfo App{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    App.pApplicationName = "RelightEngine";
    App.pEngineName = "RelightEngine";
    App.apiVersion = VK_MAKE_API_VERSION(
        0, RE_VK_REQUIRED_VERSION_MAJOR, RE_VK_REQUIRED_VERSION_MINOR, 0);

    VkInstanceCreateInfo InstanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    InstanceCreateInfo.pApplicationInfo = &App;
    InstanceCreateInfo.enabledExtensionCount = RequiredExtensions.size();
    InstanceCreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();
    InstanceCreateInfo.enabledLayerCount = RequestedLayers.size();
    InstanceCreateInfo.ppEnabledLayerNames = RequestedLayers.data();

    if(InstanceExtensions.contains(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
        InstanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

#if RE_VALIDATION_LAYERS
    VkDebugReportCallbackCreateInfoEXT DebugReportCreateInfo = {
        VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT};
    DebugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                  VK_DEBUG_REPORT_WARNING_BIT_EXT;
    DebugReportCreateInfo.pfnCallback = DebugReportCallbackFunc;

    if(InstanceExtensions.contains(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
        InstanceCreateInfo.pNext = &DebugReportCreateInfo;
    }
#endif

    // Create the Vulkan instance
    vk_check(vkCreateInstance(&InstanceCreateInfo, nullptr, &DeviceInfo.Instance));

    BindVkInstance(DeviceInfo.Instance);

#if RE_VALIDATION_LAYERS
    if(InstanceExtensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = DebugUtilCallback};

        vk_check(vkCreateDebugUtilsMessengerEXT(
            DeviceInfo.Instance, &DebugUtilsMessengerCreateInfo, nullptr,
            &DebugReportInfo.DebugMessenger));
    } else {
        vk_check(vkCreateDebugReportCallbackEXT(
            DeviceInfo.Instance, &DebugReportCreateInfo, nullptr,
            &DebugReportInfo.DebugReportCallback));
    }
#endif
}

constexpr uint32_t const INVALID_VK_INDEX = 0xFFFFFFFF;

uint32_t identifyGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice) {
    uint32_t queueFamilyPropCount;
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queueFamilyPropCount, nullptr);
    checkf(queueFamilyPropCount > 0, "No queue family prop found.");

    std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamilyPropCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queueFamilyPropCount, queueFamiliesProperties.data());

    uint32_t graphicsQueueFamilyIndex = INVALID_VK_INDEX;
    for(uint32_t j = 0; j < queueFamiliesProperties.size(); ++j) {
        VkQueueFamilyProperties props = queueFamiliesProperties[j];
        if(props.queueCount != 0 && props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex = j;
            break;
        }
    }
    return graphicsQueueFamilyIndex;
}
inline int deviceTypeOrder(VkPhysicalDeviceType deviceType) {
    switch(deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return 5;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return 4;
        case VK_PHYSICAL_DEVICE_TYPE_CPU: return 3;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return 2;
        case VK_PHYSICAL_DEVICE_TYPE_OTHER: return 1;
        default:
            RE_LOGW("devcieTypeOrder: Unexpected deviceType: {}", deviceType);
            return -1;
    }
}
VkPhysicalDevice selectPhysicalDevice(VkInstance instance) {
    uint32_t gpuCount = 0;
    vk_check(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));
    checkf(gpuCount > 0, "No physical device found.");
    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    vk_check(vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data()));

    struct DeviceInfo {
        VkPhysicalDevice device = VK_NULL_HANDLE;
        VkPhysicalDeviceType deviceType = VK_PHYSICAL_DEVICE_TYPE_OTHER;
        int8_t index = -1;
        std::string_view name;
    };
    std::vector<DeviceInfo> deviceList(physicalDevices.size());

    for(size_t deviceInd = 0; deviceInd < physicalDevices.size(); ++deviceInd) {
        auto const candidateDevice = physicalDevices[deviceInd];
        VkPhysicalDeviceProperties targetDeviceProperties;
        vkGetPhysicalDeviceProperties(candidateDevice, &targetDeviceProperties);

        int const major = VK_VERSION_MAJOR(targetDeviceProperties.apiVersion);
        int const minor = VK_VERSION_MINOR(targetDeviceProperties.apiVersion);

        // Does the device support the required Vulkan level?
        if(major < RE_VK_REQUIRED_VERSION_MAJOR) { continue; }
        if(major == RE_VK_REQUIRED_VERSION_MAJOR &&
           minor < RE_VK_REQUIRED_VERSION_MINOR) {
            continue;
        }

        // Does the device have any command queues that support graphics?
        // In theory, we should also ensure that the device supports presentation of our
        // particular VkSurface, but we don't have a VkSurface yet, so we'll skip this requirement.
        if(identifyGraphicsQueueFamilyIndex(candidateDevice) == INVALID_VK_INDEX) {
            continue;
        }

        // Does the device support the VK_KHR_swapchain extension?
        uint32_t deviceExtensionCount;
        vkEnumerateDeviceExtensionProperties(
            candidateDevice, nullptr, &deviceExtensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(deviceExtensionCount);
        vkEnumerateDeviceExtensionProperties(
            candidateDevice, nullptr, &deviceExtensionCount, extensions.data());

        bool supportsSwapchain = false;
        for(auto const &extension: extensions) {
            if(!strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
                supportsSwapchain = true;
                break;
            }
        }
        if(!supportsSwapchain) { continue; }
        deviceList[deviceInd].device = candidateDevice;
        deviceList[deviceInd].deviceType = targetDeviceProperties.deviceType;
        deviceList[deviceInd].index = deviceInd;
        deviceList[deviceInd].name = targetDeviceProperties.deviceName;
    }

    // Sort the found devices
    std::sort(
        deviceList.begin(), deviceList.end(),
        [](DeviceInfo const &a, DeviceInfo const &b) {
            if(b.device == VK_NULL_HANDLE) { return false; }
            if(a.device == VK_NULL_HANDLE) { return true; }
            return deviceTypeOrder(a.deviceType) < deviceTypeOrder(b.deviceType);
        });
    auto device = deviceList.back().device;
    checkf(device != VK_NULL_HANDLE, "Unable to find suitable device.");
    return device;
}

FExtensionSet getDeviceExtensions(VkPhysicalDevice device) {
    std::string_view const TARGET_EXTS[] = {
        "VK_KHR_portability_subset",
        VK_KHR_MAINTENANCE1_EXTENSION_NAME,
        VK_KHR_MAINTENANCE2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE3_EXTENSION_NAME,
    };
    FExtensionSet exts;
    // Identify supported physical device extensions
    uint32_t deviceExtensionCount;
    vk_check(vkEnumerateDeviceExtensionProperties(
        device, nullptr, &deviceExtensionCount, nullptr));
    std::vector<VkExtensionProperties> extensions(deviceExtensionCount);
    vk_check(vkEnumerateDeviceExtensionProperties(
        device, nullptr, &deviceExtensionCount, extensions.data()));
    for(auto const &extension: extensions) {
        for(auto const &targetExt: TARGET_EXTS) {
            if(targetExt == extension.extensionName) { exts.insert(targetExt); }
        }
    }
    return exts;
}

void FRHI::CreateDevice() {
    DeviceInfo.PhysicalDevice = selectPhysicalDevice(DeviceInfo.Instance);
    DeviceInfo.GraphicsQueueIndex =
        int32_t(identifyGraphicsQueueFamilyIndex(DeviceInfo.PhysicalDevice));

    vkGetPhysicalDeviceProperties(
        DeviceInfo.PhysicalDevice, &DeviceInfo.PhysicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(
        DeviceInfo.PhysicalDevice, &DeviceInfo.PhysicalDeviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(
        DeviceInfo.PhysicalDevice, &DeviceInfo.MemoryProperties);

    RE_LOGI(
        "Vulkan Physical Device Name: {}",
        DeviceInfo.PhysicalDeviceProperties.deviceName);
    RE_LOGI(
        "Vulkan Physical Device Info: apiVersion: {} \n\t driverVersion: {}",
        DeviceInfo.PhysicalDeviceProperties.apiVersion,
        DeviceInfo.PhysicalDeviceProperties.driverVersion);
    RE_LOGI(
        "API Version Supported: {}.{}.{}",
        VK_VERSION_MAJOR(DeviceInfo.PhysicalDeviceProperties.apiVersion),
        VK_VERSION_MINOR(DeviceInfo.PhysicalDeviceProperties.apiVersion),
        VK_VERSION_PATCH(DeviceInfo.PhysicalDeviceProperties.apiVersion));

    FExtensionSet deviceExts = getDeviceExtensions(DeviceInfo.PhysicalDevice);

    float queuePriority = 1.0f;

    // Create one queue
    VkDeviceQueueCreateInfo queueCreateInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queueCreateInfo.queueFamilyIndex = DeviceInfo.GraphicsQueueIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // We could simply enable all supported features, but since that may have performance
    // consequences let's just enable the features we need.
    VkPhysicalDeviceFeatures enabledFeatures{
        .samplerAnisotropy = DeviceInfo.PhysicalDeviceFeatures.samplerAnisotropy,
        .textureCompressionETC2 =
            DeviceInfo.PhysicalDeviceFeatures.textureCompressionETC2,
        .textureCompressionBC = DeviceInfo.PhysicalDeviceFeatures.textureCompressionBC,
    };

    std::vector<const char *> requestExtensions;
    requestExtensions.reserve(deviceExts.size() + 1);

    requestExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    for(auto ext: deviceExts) {
        requestExtensions.push_back(ext.data());
    }

    VkDeviceCreateInfo deviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
    deviceCreateInfo.enabledExtensionCount = requestExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = requestExtensions.data();

    vk_check(vkCreateDevice(
        DeviceInfo.PhysicalDevice, &deviceCreateInfo, nullptr, &DeviceInfo.Device));

    vkGetDeviceQueue(
        DeviceInfo.Device, DeviceInfo.GraphicsQueueIndex, 0, &DeviceInfo.GraphicsQueue);
    checkf(DeviceInfo.GraphicsQueue != VK_NULL_HANDLE, "Unable to get graphics queue.");
}
void FRHI::CreateSurface(void *nativeWindow) {
    VkSurfaceKHR surface;

    VkInstance instance = DeviceInfo.Instance;
    // On certain platforms, the extent of the surface cannot be queried from Vulkan. In those
    // situations, we allow the frontend to pass in the extent to use in creating the swap
    // chains. Platform implementation should set extent to 0 if they do not expect to set the
    // swap chain extent.
    VkExtent2D extent;

#if defined(__ANDROID__)
    VkAndroidSurfaceCreateInfoKHR const createInfo{
        .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
        .window = (ANativeWindow *)nativeWindow,
    };
    VkResult const result = vkCreateAndroidSurfaceKHR(
        instance, &createInfo, VKALLOC, (VkSurfaceKHR *)&surface);
    ASSERT_POSTCONDITION(result == VK_SUCCESS, "vkCreateAndroidSurfaceKHR error.");
#elif defined(__linux__) && defined(FILAMENT_SUPPORTS_GGP)
    VkStreamDescriptorSurfaceCreateInfoGGP const surface_create_info = {
        .sType = VK_STRUCTURE_TYPE_STREAM_DESCRIPTOR_SURFACE_CREATE_INFO_GGP,
        .streamDescriptor = kGgpPrimaryStreamDescriptor,
    };
    PFN_vkCreateStreamDescriptorSurfaceGGP fpCreateStreamDescriptorSurfaceGGP =
        reinterpret_cast<PFN_vkCreateStreamDescriptorSurfaceGGP>(
            vkGetInstanceProcAddr(instance, "vkCreateStreamDescriptorSurfaceGGP"));
    ASSERT_PRECONDITION(
        fpCreateStreamDescriptorSurfaceGGP != nullptr,
        "Error getting VkInstance "
        "function vkCreateStreamDescriptorSurfaceGGP");
    VkResult const result = fpCreateStreamDescriptorSurfaceGGP(
        instance, &surface_create_info, nullptr, (VkSurfaceKHR *)&surface);
    ASSERT_POSTCONDITION(
        result == VK_SUCCESS, "vkCreateStreamDescriptorSurfaceGGP error.");
#elif defined(__linux__) && defined(FILAMENT_SUPPORTS_WAYLAND)
    wl *ptrval = reinterpret_cast<wl *>(nativeWindow);
    extent.width = ptrval->width;
    extent.height = ptrval->height;

    VkWaylandSurfaceCreateInfoKHR const createInfo = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = ptrval->display,
        .surface = ptrval->surface,
    };
    VkResult const result = vkCreateWaylandSurfaceKHR(
        instance, &createInfo, VKALLOC, (VkSurfaceKHR *)&surface);
    ASSERT_POSTCONDITION(result == VK_SUCCESS, "vkCreateWaylandSurfaceKHR error.");
#elif LINUX_OR_FREEBSD && defined(FILAMENT_SUPPORTS_X11)
    if(g_x11_vk.library == nullptr) {
        g_x11_vk.library = dlopen(LIBRARY_X11, RTLD_LOCAL | RTLD_NOW);
        ASSERT_PRECONDITION(g_x11_vk.library, "Unable to open X11 library.");
    #if defined(FILAMENT_SUPPORTS_XCB)
        g_x11_vk.xcbConnect = (XCB_CONNECT)dlsym(g_x11_vk.library, "xcb_connect");
        int screen;
        g_x11_vk.connection = g_x11_vk.xcbConnect(nullptr, &screen);
        ASSERT_POSTCONDITION(
            vkCreateXcbSurfaceKHR, "Unable to load vkCreateXcbSurfaceKHR function.");
    #endif
    #if defined(FILAMENT_SUPPORTS_XLIB)
        g_x11_vk.openDisplay = (X11_OPEN_DISPLAY)dlsym(g_x11_vk.library, "XOpenDisplay");
        g_x11_vk.display = g_x11_vk.openDisplay(NULL);
        ASSERT_PRECONDITION(g_x11_vk.display, "Unable to open X11 display.");
        ASSERT_POSTCONDITION(
            vkCreateXlibSurfaceKHR, "Unable to load vkCreateXlibSurfaceKHR function.");
    #endif
    }
    #if defined(FILAMENT_SUPPORTS_XCB) || defined(FILAMENT_SUPPORTS_XLIB)
    bool useXcb = false;
    #endif
    #if defined(FILAMENT_SUPPORTS_XCB)
        #if defined(FILAMENT_SUPPORTS_XLIB)
    useXcb = (flags & SWAP_CHAIN_CONFIG_ENABLE_XCB) != 0;
        #else
    useXcb = true;
        #endif
    if(useXcb) {
        VkXcbSurfaceCreateInfoKHR const createInfo = {
            .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
            .connection = g_x11_vk.connection,
            .window = (xcb_window_t) reinterpret_cast<uint64_t>(nativeWindow),
        };
        VkResult const result = vkCreateXcbSurfaceKHR(
            instance, &createInfo, VKALLOC, (VkSurfaceKHR *)&surface);
        ASSERT_POSTCONDITION(result == VK_SUCCESS, "vkCreateXcbSurfaceKHR error.");
    }
    #endif
    #if defined(FILAMENT_SUPPORTS_XLIB)
    if(!useXcb) {
        VkXlibSurfaceCreateInfoKHR const createInfo = {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .dpy = g_x11_vk.display,
            .window = (Window)nativeWindow,
        };
        VkResult const result = vkCreateXlibSurfaceKHR(
            instance, &createInfo, VKALLOC, (VkSurfaceKHR *)&surface);
        ASSERT_POSTCONDITION(result == VK_SUCCESS, "vkCreateXlibSurfaceKHR error.");
    }
    #endif
#elif defined(WIN32)
    VkWin32SurfaceCreateInfoKHR const createInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = GetModuleHandle(nullptr),
        .hwnd = (HWND)nativeWindow,
    };
    vk_check(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface));
#endif
    DeviceInfo.Surface = surface;
}
}