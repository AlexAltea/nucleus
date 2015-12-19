/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_fence.h"

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

void Direct3D12Fence::wait() {
}

void Direct3D12Fence::wait(Clock::duration timeout) {
}

}  // namespace direct3d12
}  // namespace gfx
