/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/backend/vulkan/vulkan.h"
#include "nucleus/graphics/backend/vulkan/vulkan_debug.h"

namespace gfx {
namespace vulkan {

class VulkanBackend : public GraphicsBackend {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    VkDevice device;

public:
    // Debugging
    VulkanDebug debug;

    VulkanBackend();
    ~VulkanBackend();

    virtual bool initialize(const BackendParameters& params) override;

    virtual CommandBuffer* createCommandBuffer() override;
    virtual Fence* createFence(const FenceDesc& desc) override;
    virtual Heap* createHeap(const HeapDesc& desc) override;
    virtual ColorTarget* createColorTarget(Texture* texture) override;
    virtual DepthStencilTarget* createDepthStencilTarget(Texture* texture) override;
    virtual Pipeline* createPipeline(const PipelineDesc& desc) override;
    virtual Shader* createShader(const ShaderDesc& desc) override;
    virtual Texture* createTexture(const TextureDesc& desc) override;
    virtual VertexBuffer* createVertexBuffer(const VertexBufferDesc& desc) override;
    virtual CommandQueue* getGraphicsCommandQueue() override;
    virtual bool doResizeBuffers(int width, int height) override;
    virtual bool doSwapBuffers() override;
};

}  // namespace vulkan
}  // namespace gfx
