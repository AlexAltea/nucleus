/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_command_queue.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_command_buffer.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_fence.h"

namespace gfx {
namespace direct3d12 {

Direct3D12CommandQueue::Direct3D12CommandQueue() {
}

Direct3D12CommandQueue::~Direct3D12CommandQueue() {
}

bool Direct3D12CommandQueue::initialize(ID3D12Device* device) {
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue));
    return true;
}

bool Direct3D12CommandQueue::initialize(ID3D12CommandQueue* newQueue) {
    if (!newQueue) {
        return false;
    }
    queue = newQueue;
    return true;
}

void Direct3D12CommandQueue::submit(CommandBuffer* cmdBuffer, Fence* fence) {
    auto* cmdList = dynamic_cast<Direct3D12CommandBuffer*>(cmdBuffer);

    ID3D12CommandList* ppCommandLists[] = { cmdList->list };
    queue->ExecuteCommandLists(1, ppCommandLists);

    auto* d3dFence = static_cast<Direct3D12Fence*>(fence);
    if (d3dFence) {
        d3dFence->signal(queue);
    }
}

void Direct3D12CommandQueue::waitIdle() {
}

}  // namespace direct3d12
}  // namespace gfx
