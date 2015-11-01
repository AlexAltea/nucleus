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

void Direct3D12CommandQueue::submit(ICommandBuffer* cmdBuffer) {
    auto* cmdList = dynamic_cast<Direct3D12CommandBuffer*>(cmdBuffer);

    ID3D12CommandList* ppCommandLists[] = { cmdList->list };
    queue->ExecuteCommandLists(1, ppCommandLists);
}

}  // namespace gfx
