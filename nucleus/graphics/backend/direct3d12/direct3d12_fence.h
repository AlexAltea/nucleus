/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/fence.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {
namespace direct3d12 {

class Direct3D12Fence : public Fence {
private:
    ID3D12Fence* fence;
    HANDLE fenceEvent;
    unsigned long long fenceValue;

public:
    Direct3D12Fence();
    ~Direct3D12Fence();

    bool initialize(ID3D12Device* device);

    // Trigger the signal status
    void signal(ID3D12CommandQueue* queue);

    virtual void wait() override;
    virtual void wait(Clock::duration timeout) override;
};

}  // namespace direct3d12
}  // namespace gfx
