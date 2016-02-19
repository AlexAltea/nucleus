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
}

bool VulkanBackend::initialize(const BackendParameters& params) {
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
