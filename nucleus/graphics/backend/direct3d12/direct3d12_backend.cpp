/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_backend.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_command_buffer.h"

namespace gfx {

Direct3D12Backend::Direct3D12Backend() {
}

Direct3D12Backend::~Direct3D12Backend() {
}

ICommandQueue* Direct3D12Backend::createCommandQueue() {
    return nullptr;
}

ICommandBuffer* Direct3D12Backend::createCommandBuffer() {
    ICommandBuffer* commandBuffer = new Direct3D12CommandBuffer();
    return commandBuffer;
}

void Direct3D12Backend::createPipeline() {
}

void Direct3D12Backend::createShader() {
}

}  // namespace gfx
