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
    D3D11_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D11_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D11_COMMAND_LIST_TYPE_DIRECT;

    device->CreateCommandQueue(nullptr, IID_PPV_ARGS(&queue));
    return true;
}

void Direct3D11CommandQueue::submit(ICommandBuffer* cmdBuffer) {
    auto* cmdList = dynamic_cast<Direct3D11CommandBuffer*>(cmdBuffer);

    ID3D11CommandList* ppCommandLists[] = { cmdList->list };
    queue->ExecuteCommandLists(1, ppCommandLists);
}

void Direct3D11CommandQueue::waitIdle() {
}

}  // namespace gfx
