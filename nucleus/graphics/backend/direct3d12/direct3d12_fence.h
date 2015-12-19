/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/fence.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {

class Direct3D12Fence : public Fence {
private:
    ID3D12Fence* fence;
    HANDLE fenceEvent;
    unsigned long long fenceValue;

public:
    Direct3D12Fence();
    ~Direct3D12Fence();

    bool initialize(ID3D12Device* device);

    virtual void wait() override;
    virtual void wait(Clock::duration timeout) override;
};

}  // namespace gfx
