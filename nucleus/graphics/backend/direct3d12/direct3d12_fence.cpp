/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_fence.h"
#include "nucleus/logger/logger.h"


namespace gfx {
namespace direct3d12 {

Direct3D12Fence::Direct3D12Fence() {
}

Direct3D12Fence::~Direct3D12Fence() {
}

bool Direct3D12Fence::initialize(ID3D12Device* device) {
    HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    if (FAILED(hr)) {
        return false;
    }
    fenceValue = 1;
    fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
    if (!fenceEvent) {
        return false;
    }
    return true;
}

void Direct3D12Fence::signal(ID3D12CommandQueue* queue) {
    queue->Signal(fence, fenceValue);
    fenceValue += 1;
}

void Direct3D12Fence::wait() {
    UINT64 fenceToWaitFor = fenceValue - 1;
    if (fence->GetCompletedValue() < fenceToWaitFor) {
        HRESULT hr = fence->SetEventOnCompletion(fenceToWaitFor, fenceEvent);
		if(FAILED(hr)) {
            logger.error(LOG_GRAPHICS, "Direct3D12Fence::wait: SetEventOnCompletion failed");
			return;
		}
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}

void Direct3D12Fence::wait(Clock::duration timeout) {
    UINT64 fenceToWaitFor = fenceValue - 1;
    DWORD ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
    if (fence->GetCompletedValue() < fenceToWaitFor) {
        HRESULT hr = fence->SetEventOnCompletion(fenceToWaitFor, fenceEvent);
		if(FAILED(hr)) {
            logger.error(LOG_GRAPHICS, "Direct3D12Fence::wait: SetEventOnCompletion failed");
			return;
		}
        WaitForSingleObject(fenceEvent, ms);
    }
}

}  // namespace direct3d12
}  // namespace gfx
