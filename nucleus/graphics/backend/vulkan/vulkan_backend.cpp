/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vulkan_backend.h"
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

VulkanBackend::VulkanBackend() : IBackend() {
}

VulkanBackend::~VulkanBackend() {
    if (device) {
        /*vkDestroyDevice(device, nullptr);*/
        device = nullptr;
    }
}

bool VulkanBackend::initialize(const BackendParameters& params) {
    /*VkApplicationInfo applicationInfo;
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
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
    instanceInfo.ppEnabledLayerNames = enabledLayers.data();
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkResult vkr = vkCreateInstance(&instanceInfo, nullptr, &instance);
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
#ifdef NUCLEUS_TARGET_WINDOWS
    VkWin32SurfaceCreateInfoKHR surfaceInfo;
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.flags = 0;
    surfaceInfo.hinstance = params.hinstance;
    surfaceInfo.hwnd = params.hwnd;
    auto err = vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface);
#endif

    Size queueCount = 1;
    std::vector<float> queuePriorities(queueCount);
    queuePriorities[0] = 1.0f;

    VkDeviceQueueCreateInfo queueInfo;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.flags = 0;
    //queueInfo.queueFamilyIndex = ideal_queue_family_index;
    queueInfo.queueCount = queueCount;
    queueInfo.pQueuePriorities = queuePriorities.data();

    VkDeviceCreateInfo deviceInfo;
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = nullptr;
    deviceInfo.flags = 0;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
    deviceInfo.ppEnabledLayerNames = enabledLayers.data();
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    deviceInfo.ppEnabledExtensionNames = enabledExtensions.data();
    deviceInfo.pEnabledFeatures = &enabledFeatures;
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
    }*/

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
