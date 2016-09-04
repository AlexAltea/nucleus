/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vulkan_backend.h"
#include "nucleus/assert.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/vulkan/vulkan.h"
//#include "nucleus/graphics/backend/vulkan/vulkan_convert.h"

//#include "nucleus/graphics/backend/vulkan/vulkan_command_buffer.h"
//#include "nucleus/graphics/backend/vulkan/vulkan_command_queue.h"
//#include "nucleus/graphics/backend/vulkan/vulkan_fence.h"
//#include "nucleus/graphics/backend/vulkan/vulkan_heap.h"
//#include "nucleus/graphics/backend/vulkan/vulkan_pipeline.h"
//#include "nucleus/graphics/backend/vulkan/vulkan_resource.h"
#include "nucleus/graphics/backend/vulkan/vulkan_shader.h"
//#include "nucleus/graphics/backend/vulkan/vulkan_target.h"
//#include "nucleus/graphics/backend/vulkan/vulkan_texture.h"
//#include "nucleus/graphics/backend/vulkan/vulkan_vertex_buffer.h"

namespace gfx {
namespace vulkan {

VulkanBackend::VulkanBackend() : GraphicsBackend() {
}

VulkanBackend::~VulkanBackend() {
    if (device) {
        /*vkDestroyDevice(device, nullptr);*/
        device = nullptr;
    }
}

bool VulkanBackend::initialize(const BackendParameters& params) {
    assert_true(initializeVulkan());

    // Set Vulkan validation layers and extensions
    debug.enable();
    std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
#if defined(VK_USE_TARGET_ANDROID_KHR)
    enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    enabledExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    VkApplicationInfo applicationInfo;
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = "Nucleus";
    applicationInfo.applicationVersion = 1;
    applicationInfo.pEngineName = "nucleus::gfx";
    applicationInfo.engineVersion = 1;
    applicationInfo.apiVersion = VK_API_VERSION;

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(debug.validationLayers.size());
    instanceInfo.ppEnabledLayerNames = debug.validationLayers.data();
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkResult vkr;
    vkr = vkCreateInstance(&instanceInfo, nullptr, &instance);
    if (vkr != VK_SUCCESS) {
        switch (vkr) {
        case VK_ERROR_INITIALIZATION_FAILED:
            logger.error(LOG_GRAPHICS, "vkCreateInstance failed: Returned VK_ERROR_INITIALIZATION_FAILED"); break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            logger.error(LOG_GRAPHICS, "vkCreateInstance failed: Returned VK_ERROR_INCOMPATIBLE_DRIVER"); break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            logger.error(LOG_GRAPHICS, "vkCreateInstance failed: Returned VK_ERROR_EXTENSION_NOT_PRESENT"); break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            logger.error(LOG_GRAPHICS, "vkCreateInstance failed: Returned VK_ERROR_LAYER_NOT_PRESENT"); break;
        default:
            logger.error(LOG_GRAPHICS, "vkCreateInstance failed: Unknown reason (%d)", vkr);
        }
        return false;
    }

    VkSurfaceKHR surface = nullptr;
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    VkXlibSurfaceCreateInfoKHR surfaceInfo;
    surfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.flags = 0;
    surfaceInfo.dpy = params.display;
    surfaceInfo.window = params.window;
    vkr = vkCreateXlibSurfaceKHR(instance, &surfaceInfo, nullptr, &surface);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    VkWin32SurfaceCreateInfoKHR surfaceInfo;
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.flags = 0;
    surfaceInfo.hinstance = params.hinstance;
    surfaceInfo.hwnd = params.hwnd;
    vkr = vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface);
#endif
    assert_true(vkr == VK_SUCCESS);

    // Physical device
    uint32_t gpuCount = 0;
    vkr = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    if (vkr != VK_SUCCESS || gpuCount == 0) {
        logger.error(LOG_GRAPHICS, "vkEnumeratePhysicalDevices failed or no compatible GPU detected");
        return false;
    }

    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    vkr = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
    if (vkr != VK_SUCCESS) {
        logger.error(LOG_GRAPHICS, "vkEnumeratePhysicalDevices failed: Unknown reason (%d)", vkr);
        return false;
    }

    uint32_t queueCount;
    physicalDevice = physicalDevices[0];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);
    assert_true(queueCount >= 1);


    std::vector<VkQueueFamilyProperties> queueProps(queueCount);
    uint32_t graphicsQueueIndex;
    for (graphicsQueueIndex = 0; graphicsQueueIndex < queueCount; graphicsQueueIndex++) {
        if (queueProps[graphicsQueueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            break;
        }
    }

    std::vector<float> queuePriorities(queueCount);
    queuePriorities[0] = 1.0f;

    VkDeviceQueueCreateInfo queueInfo;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = graphicsQueueIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities.data();

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = nullptr;
    deviceInfo.flags = 0;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = static_cast<uint32_t>(debug.validationLayers.size());
    deviceInfo.ppEnabledLayerNames = debug.validationLayers.data();
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    deviceInfo.ppEnabledExtensionNames = enabledExtensions.data();
    deviceInfo.pEnabledFeatures = nullptr;
    vkr = vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device);
    if (vkr != VK_SUCCESS) {
        switch (vkr) {
        case VK_ERROR_INITIALIZATION_FAILED:
            logger.error(LOG_GRAPHICS, "vkCreateDevice failed: Returned VK_ERROR_INITIALIZATION_FAILED"); break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            logger.error(LOG_GRAPHICS, "vkCreateDevice failed: Returned VK_ERROR_EXTENSION_NOT_PRESENT"); break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            logger.error(LOG_GRAPHICS, "vkCreateDevice failed: Returned VK_ERROR_LAYER_NOT_PRESENT"); break;
        default:
            logger.error(LOG_GRAPHICS, "vkCreateDevice failed: Unknown reason (%d)", vkr);
        }
        return false;
    }

    // Store device properties
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

    return true;
}

CommandBuffer* VulkanBackend::createCommandBuffer() {
    return nullptr;
}

Fence* VulkanBackend::createFence(const FenceDesc& desc) {
    return nullptr;
}

Heap* VulkanBackend::createHeap(const HeapDesc& desc) {
    return nullptr;
}

ColorTarget* VulkanBackend::createColorTarget(Texture* texture) {
    return nullptr;
}

DepthStencilTarget* VulkanBackend::createDepthStencilTarget(Texture* texture) {
    return nullptr;
}

Pipeline* VulkanBackend::createPipeline(const PipelineDesc& desc) {
    return nullptr;
}

Shader* VulkanBackend::createShader(const ShaderDesc& desc) {
    return nullptr;
}

Texture* VulkanBackend::createTexture(const TextureDesc& desc) {
    return nullptr;
}

VertexBuffer* VulkanBackend::createVertexBuffer(const VertexBufferDesc& desc) {
    return nullptr;
}

CommandQueue* VulkanBackend::getGraphicsCommandQueue() {
    return nullptr;
}

bool VulkanBackend::doResizeBuffers(int width, int height) {
    return true;
}

bool VulkanBackend::doSwapBuffers() {
    return true;
}

}  // namespace vulkan
}  // namespace gfx
