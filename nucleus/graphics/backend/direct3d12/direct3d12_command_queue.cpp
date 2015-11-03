/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_command_queue.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_command_buffer.h"

namespace gfx {

Direct3D12CommandQueue::Direct3D12CommandQueue() {
}

Direct3D12CommandQueue::~Direct3D12CommandQueue() {
}

bool Direct3D12CommandQueue::initialize(ID3D12Device* device) {
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    device->CreateCommandQueue(nullptr, IID_PPV_ARGS(&queue));
    return true;
}

void Direct3D12CommandQueue::submit(ICommandBuffer* cmdBuffer) {
    auto* cmdList = dynamic_cast<Direct3D12CommandBuffer*>(cmdBuffer);

    ID3D12CommandList* ppCommandLists[] = { cmdList->list };
    queue->ExecuteCommandLists(1, ppCommandLists);
}

void Direct3D12CommandQueue::waitIdle() {
}

}  // namespace gfx
