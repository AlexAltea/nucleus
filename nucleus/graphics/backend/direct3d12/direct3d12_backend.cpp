/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_backend.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_command_buffer.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_command_queue.h"

namespace gfx {

Direct3D12Backend::Direct3D12Backend() : IBackend() {
}

Direct3D12Backend::~Direct3D12Backend() {
}

bool Direct3D12Backend::initialize(DisplayHandler display) {
    D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
}

ICommandQueue* Direct3D12Backend::createCommandQueue() {
    auto* commandQueue = new Direct3D12CommandQueue();

    if (!commandQueue->initialize(device)) {
        logger.error(LOG_GRAPHICS, "OpenGLBackend::createCommandQueue: Could not initialize OpenGLCommandQueue");
        return nullptr;
    }
    return commandQueue;
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
