/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d11_command_queue.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11_command_buffer.h"

namespace gfx {

Direct3D11CommandQueue::Direct3D11CommandQueue() {
}

Direct3D11CommandQueue::~Direct3D11CommandQueue() {
}

bool Direct3D11CommandQueue::initialize(ID3D11Device* device) {
    // TODO
    return true;
}

void Direct3D11CommandQueue::submit(CommandBuffer* cmdBuffer, Fence* fence) {
    auto* cmdList = dynamic_cast<Direct3D11CommandBuffer*>(cmdBuffer);

    // TODO
    immediateContext->ExecuteCommandList(nullptr, TRUE);
}

void Direct3D11CommandQueue::waitIdle() {
}

}  // namespace gfx
